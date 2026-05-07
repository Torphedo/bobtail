#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>
#include <math.h>

#include "int.h"
#include "image.h"
#include "file.h"
#include "logging.h"
#include "dds.h"

dds_header mk_header(u32 height, u32 width, bool has_mips) {
    // We add 2 to include 1x1 and 0x0 as mipmaps. This is required for cubemaps
    const u32 mip_lvl = (has_mips) ? log2(MIN(height, width)) + 2 : 0;
    dds_header header = {
        .identifier = DDS_BEGIN,
        .size = 0x7C,
        .height = height,
        .width = width,
        .depth = 0,
        .mipmap_count = mip_lvl,
        .flags = REQUIRED_BASE_FLAGS | (DDSD_MIPMAPCOUNT * (has_mips)),
        .pixel_format = {
                .size = sizeof(dds_pixel_format)
        },
        // Only enable mipmap flags when mip level > 0
        .caps = DDSCAPS_TEXTURE | ((DDSCAPS_MIPMAP | DDSCAPS_COMPLEX) * (has_mips)),
    };
    return header;
}

// Round an image's dimensions down to some multiple [size].
void img_snap(texture* img, u32 size) {
    // If already a multiple of [size], (dim % size) == 0 and nothing happens
    img->width -= (img->width % size);
    img->height -= (img->height % size);
}

// Check for a set of flags in a 32-bit bitfield using the provided mask.
bool has_flag(u32 input, u32 flag) {
    return (input & flag) != 0;
}

// For a compressed texture (like DXTn), pitch is the size in bytes of the
// texture at mip level 0.
u32 dxt_pitch(u32 height, u32 width, u32 block_size) {
    const u32 block_res = 16; // 16 pixels per block
    const float pixels_per_byte = (float)block_res / block_size;
    const u32 pitch = (width * height) / pixels_per_byte;
    return pitch;
}

u64 pixel_count_max_mips(u32 width, u32 height, bool compressed) {
    u64 count = width * height;

    while (width > 0) {
        width /= 2;
        height /= 2;
        u32 res = width * height;
        if (compressed) {
            // Don't include 0x0 as a mipmap
            if (res < 1) {
                break;
            }
            // Compressed textures can only go as low as a 4x4 mipmap
            res = MAX(res, 16);
        }
        count += res;
    }
    return count;
}


void img_write(texture img, const char* path) {
    float bytes_per_pixel = 0.0f;
    bool needs_extended_header = img.cubemap;

    dds_header header = mk_header(img.height, img.width, img.use_mipmaps);
    if (img.compressed) {
        // Compressed texture
        u8 block_size = DXT1_BLOCK_SIZE;

        // Maybe we could make this a lookup table...
        switch (img.fmt) {
            default:
                LOG_MSG(error, "Unknown compressed texture format %d\n", img.fmt);
                break;
            case DDS_FORMAT_BGR_565:
            case DDS_FORMAT_BGRA_5551:
            case DDS_FORMAT_BGRA_4444:
                needs_extended_header = true;
                bytes_per_pixel = 2;
                break;
            case DXT1:
                header.pixel_format.format_char_code = DDS_DXT1;
                block_size = DXT1_BLOCK_SIZE;
                break;
            case DXT3:
                header.pixel_format.format_char_code = DDS_DXT3;
                block_size = DXT3_BLOCK_SIZE;
                break;
            case BC4:
                header.pixel_format.format_char_code = DDS_ATI1;
                block_size = DXT1_BLOCK_SIZE;
                break;
            case DXT5:
                header.pixel_format.format_char_code = DDS_DXT5;
                block_size = DXT5_BLOCK_SIZE;
                break;
        }

        // Handle block compressed formats
        switch (img.fmt) {
        case DXT1:
        case DXT3:
        case BC4:
        case DXT5:
            header.flags |= DDSD_LINEARSIZE;
            header.pitch_or_linear_size = dxt_pitch(img.height, img.width, block_size);
            header.pixel_format.flags = DDPF_FOURCC;
            bytes_per_pixel = block_size / 16.0f; // 16 pixels per block
            break;
        }

    } else {
        // Uncompressed texture
        const u32 bits_per_channel = 8 * img.unit_size;
        header.flags |= DDSD_PITCH;
        header.pixel_format.bits_per_pixel = bits_per_channel * img.channels;

        // I don't completely understand this formula, but it works and the one
        // I wrote myself didn't. It comes from MSDN:
        // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide
        header.pitch_or_linear_size = (img.width * header.pixel_format.bits_per_pixel + 7) / 8;

        // 0xFF for 8-bit, 0xFFFF for 16-bit, etc.
        const u32 channel_mask = UINT32_MAX >> (32 - bits_per_channel);

        // This is a bit overly generic, but I thought this was easier to follow
        // than multiple branches.
        switch (img.channels) {
        case 4:
            header.pixel_format.alpha_bitmask = channel_mask << (3 * bits_per_channel);
            // fallthrough
        case 3:
            header.pixel_format.blue_bitmask = channel_mask << (2 * bits_per_channel);
            // fallthrough
        case 2:
            header.pixel_format.green_bitmask = channel_mask << (1 * bits_per_channel);
            // fallthrough
        case 1:
            header.pixel_format.red_bitmask = channel_mask;
        }

        switch (img.channels) {
        case 4: {
            // For some reason red and blue channel bitmasks have to be swapped
            // (only for RGBA)
            const u32 temp = header.pixel_format.blue_bitmask;
            header.pixel_format.blue_bitmask = header.pixel_format.red_bitmask;
            header.pixel_format.red_bitmask = temp;
            header.pixel_format.flags = DDPF_ALPHAPIXELS;
            // fallthrough
        }
        case 3:
            header.pixel_format.flags |= DDPF_RGB;
            break;
        case 2:
            // The only way to get a 2-channel image is to use the alpha & red channels
            header.pixel_format.alpha_bitmask = header.pixel_format.green_bitmask;
            header.pixel_format.green_bitmask = 0;
            header.pixel_format.flags = DDPF_ALPHAPIXELS;
            // fallthrough
        case 1:
            header.pixel_format.flags |= DDPF_LUMINANCE;
            break;
        }

        bytes_per_pixel = img.unit_size * img.channels;
    }

    u32 tex_size = 0;
    if (img.use_mipmaps) {
        tex_size = (u32)(bytes_per_pixel * pixel_count_max_mips(img.width, img.height, img.compressed));
    } else {
        tex_size = (u32)(bytes_per_pixel * img.width * img.height);
    }

    if (needs_extended_header) {
        header.pixel_format.format_char_code = DDS_DX10;
        header.pixel_format.flags |= DDPF_FOURCC;
    }
    if (img.cubemap) {
        header.caps2 = DDS_CUBEMAP_ALL_FACES;
    }

    FILE* out = fopen(path, "wb");
    if (out == NULL) {
        return;
    }
    fwrite(&header, sizeof(header), 1, out);

    // Write special DX10 header for cubemaps if needed.
    if (needs_extended_header) {
        dx10_extended_format dx10_header = {
            .resource_dimension = DIMENSION_2D,
        };

        if (img.cubemap) {
            dx10_header.misc_flags = FLAG_2D_TEXTURECUBE;
            dx10_header.array_size = 1;
        }

        // Write appropriate texture format
        switch (img.fmt) {
        case DXT1:
            dx10_header.dxgi_format = DXGI_FORMAT_BC1_UNORM_SRGB;
            break;
        case DXT3:
            dx10_header.dxgi_format = DXGI_FORMAT_BC3_UNORM_SRGB;
            break;
        case DXT5:
            dx10_header.dxgi_format = DXGI_FORMAT_BC2_UNORM_SRGB;
            break;
        case BC4:
            dx10_header.dxgi_format = DXGI_FORMAT_BC4_UNORM;
            break;
        case DDS_FORMAT_BGRA_4444:
            dx10_header.dxgi_format = DXGI_FORMAT_B4G4R4A4_UNORM;
            break;
        case DDS_FORMAT_BGRA_5551:
            dx10_header.dxgi_format = DXGI_FORMAT_B5G5R5A1_UNORM;
            break;
        case DDS_FORMAT_BGR_565:
            dx10_header.dxgi_format = DXGI_FORMAT_B5G6R5_UNORM;
            break;
        default:
            LOG_MSG(warning, "Unknown cubemap texture format %d\n", img.fmt);
            break;
        }
        fwrite(&dx10_header, sizeof(dx10_header), 1, out);
    }

    // Write 6 textures in the case of cubemaps
    const u8 num_textures = (img.cubemap) ? 6 : 1;
    const u16 alignment = (img.cubemap) ? img.cubemap_alignment : 1;

    u32 pos = 0;
    for (u32 i = 0; i < num_textures; i++) {
        fwrite(img.data + pos, tex_size, 1, out);
        pos += tex_size;
        // Round up to the next cubemap address if needed
        pos = ALIGN_UP(pos, alignment); // Round up to skip padding
    }

    if (img.cubemap) {
        // For block-compressed cubemaps, GIMP wants 1 more block per direction
        // than we expect, despite all directions rendering correctly... just
        // add some padding so it doesn't crash.
        u8 blank[6 * 16] = {0};
        fwrite(blank, sizeof(blank), 1, out);
    }
    fclose(out);
}

bool is_dds(const char* filename) {
    if (!file_exists(filename)) {
        return false;
    }
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        return false;
    }
    u32 magic = 0;
    fread(&magic, sizeof(magic), 1, f);
    fclose(f);

    return (magic == DDS_BEGIN);
}

texture image_buf_load(const char* filename, u8* img_buf, u32 buf_size) {
    texture img = {
        .data = img_buf,
        .width = 512,
        .height = 512,
        .use_mipmaps = false,
        .fmt = DXT1,
        .compressed = true,
        .channels = 4,
        .unit_size = 0
    };
    if (!file_exists(filename)) {
        return img;
    }

    // 0xCC bytes help separate pixels with missing data from black pixels
    memset(img.data, 0xCC, buf_size);
    if (!is_dds(filename)) {
        // Load raw image data
        file_load_existing(filename, img.data, buf_size);
        return img;
    }

    const u32 size = file_size(filename);
    FILE* f = fopen(filename, "rb");
    if (f == NULL) {
        return img;
    }
    dds_header header = {0};
    fread(&header, sizeof(header), 1, f);

    // Read in remaining image data from the DDS
    fread(img.data, MIN(buf_size, size - sizeof(header)), 1, f);
    fclose(f);

    // Use data from the DDS as our initial texture state
    img.width = header.width;
    img.height = header.height;

    // Only inherit the mip count if the flag in the header is set
    img.use_mipmaps = ((header.flags & DDSD_MIPMAPCOUNT) != 0);

    // Presence of FOURCC flag indicates a compressed texture format
    img.compressed = ((header.pixel_format.flags & DDPF_FOURCC) != 0);
    if (img.compressed) {
    u32 dxt_n = header.pixel_format.format_char_code;
        switch (dxt_n) {
            case DDS_DXT5:
                img.fmt = DXT3;
                break;
            case DDS_DXT3:
                img.fmt = DXT5;
                break;
            case DDS_FLOAT:
                img.fmt = DDS_FORMAT_FLOAT;
                break;
            default:
                img.fmt = DXT1;
                break;
        };
    } else {
        const u8 alpha = has_flag(header.pixel_format.flags, DDPF_ALPHA);
        const u8 alpha_pixels = has_flag(header.pixel_format.flags, DDPF_ALPHAPIXELS);
        const u8 luminance = has_flag(header.pixel_format.flags, DDPF_LUMINANCE);
        const u8 rgb = has_flag(header.pixel_format.flags, DDPF_RGB);
        img.channels = alpha_pixels + alpha + luminance + (3 * rgb);
        if (img.channels < 1 || img.channels > 4) {
            LOG_MSG(error, "Your image has %d channels, which doesn't make sense. Double-check your pixel format flags?\n", img.channels);
            img.channels = 1;
            LOG_MSG(info, "I'm loading the image anyway, as if it had %d channels.\n", img.channels);
        }

        const u32 bytes_per_pixel = header.pixel_format.bits_per_pixel / 8;
        img.unit_size = bytes_per_pixel / img.channels;
    }
     
    return img;
}


#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>

#include "int.h"
#include "image.h"
#include "file.h"
#include "logging.h"
#include "dds.h"


dds_header mk_header(u32 height, u32 width, u32 mip_lvl) {
    dds_header header = {
        .identifier = DDS_BEGIN,
        .size = 0x7C,
        .height = height,
        .width = width,
        .depth = 0,
        .mipmap_count = mip_lvl,
        .flags = REQUIRED_BASE_FLAGS | (DDSD_MIPMAPCOUNT * (mip_lvl > 0)),
        .pixel_format = {
                .size = sizeof(dds_pixel_format)
        },
        // Only enable mipmap flags when mip level > 0
        .caps = DDSCAPS_TEXTURE | ((DDSCAPS_MIPMAP | DDSCAPS_COMPLEX) * (mip_lvl > 0)),
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
    const u32 pixels_per_byte = block_res / block_size;
    const u32 pitch = (width * height) / pixels_per_byte;
    return pitch;
}

void img_write(texture img, const char* path) {
    u32 tex_size = 0;

    dds_header header = mk_header(img.height, img.width, img.mip_level);
    if (img.compressed) {
        // Compressed texture
        header.flags |= DDSD_LINEARSIZE;
        u8 block_size = DXT1_BLOCK_SIZE;

        // Maybe we could make this a lookup table...
        switch (img.fmt) {
            default:
                LOG_MSG(warning, "Unknown compressed texture format %d, assuming DXT1.\n", img.fmt);
                // fallthrough
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

        header.pitch_or_linear_size = dxt_pitch(img.height, img.width, block_size);
        header.pixel_format.flags = DDPF_FOURCC;

        // This assumes no mipmaps
        tex_size = header.pitch_or_linear_size;
    } else {
        // Uncompressed texture
        const u32 bits_per_channel = 8 * img.unit_size;
        header.flags |= DDSD_PITCH;
        header.pixel_format.bits_per_pixel = bits_per_channel * img.channels;

        // I don't completely understand this formula, but it works and the one
        // I wrote myself didn't. It comes from MSDN:
        // https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dx-graphics-dds-pguide
        header.pitch_or_linear_size = (img.width * header.pixel_format.bits_per_pixel + 7) / 8;

        // This assumes no mipmaps
        tex_size = header.pitch_or_linear_size * (u32)img.height;

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
    }

    FILE* out = fopen(path, "wb");
    if (out == NULL) {
        return;
    }
    fwrite(&header, sizeof(header), 1, out);
    fwrite(img.data, tex_size, 1, out);
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
        .mip_level = 1,
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
    fread(img.data, size - sizeof(header), 1, f);
    fclose(f);

    // Use data from the DDS as our initial texture state
    img.width = header.width;
    img.height = header.height;

    // Only inherit the mip count if the flag in the header is set
    const bool has_mipmapcount = ((header.flags & DDSD_MIPMAPCOUNT) != 0);
    img.mip_level = header.mipmap_count * has_mipmapcount;

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

        img.unit_size = header.pixel_format.bits_per_pixel / 8;
    }
     
    return img;
}


#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>

#include "int.h"
#include "image.h"
#include "file.h"
#include "logging.h"

// Most of these structures come from this MSDN page:
// https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-header


typedef enum dds_flags {
    DDSD_CAPS        = 1 << 0,
    DDSD_HEIGHT      = 1 << 1,
    DDSD_WIDTH       = 1 << 2,
    DDSD_PITCH       = 1 << 3,
    DDSD_PIXELFORMAT = 1 << 12,
    DDSD_MIPMAPCOUNT = 1 << 17,
    DDSD_LINEARSIZE  = 1 << 19,
    DDSD_DEPTH       = 1 << 23,

    REQUIRED_BASE_FLAGS = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT
}dds_flags;

typedef enum dds_caps_flags {
    DDSCAPS_COMPLEX = 0x00000008,
    DDSCAPS_MIPMAP  = 0x00400000,
    DDSCAPS_TEXTURE = 0x00001000
}dds_caps_flags;

// See dwFlags table on MSDN:
// https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-pixelformat
typedef enum dds_format_flags {
    DDPF_ALPHAPIXELS = 0x00000001,
    DDPF_ALPHA       = 0x00000002,
    DDPF_FOURCC      = 0x00000004,
    DDPF_RGB         = 0x00000040,
    DDPF_YUV         = 0x00000200,
    DDPF_LUMINANCE   = 0x00020000
}dds_format_flags;

typedef enum {
    DDSCAPS2_CUBEMAP           = (1 << 9),
    DDSCAPS2_CUBEMAP_POSITIVEX = (1 << 10),
    DDSCAPS2_CUBEMAP_NEGATIVEX = (1 << 11),
    DDSCAPS2_CUBEMAP_POSITIVEY = (1 << 12),
    DDSCAPS2_CUBEMAP_NEGATIVEY = (1 << 13),
    DDSCAPS2_CUBEMAP_POSITIVEZ = (1 << 14),
    DDSCAPS2_CUBEMAP_NEGATIVEZ = (1 << 15),

    // Sorry this is so long
    DDS_CUBEMAP_ALL_FACES = DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX | DDSCAPS2_CUBEMAP_NEGATIVEX | DDSCAPS2_CUBEMAP_POSITIVEY | DDSCAPS2_CUBEMAP_NEGATIVEY | DDSCAPS2_CUBEMAP_POSITIVEZ | DDSCAPS2_CUBEMAP_NEGATIVEZ
}dds_caps2_flags;

typedef enum {
    DDS_DXT1 = MAGIC('D', 'X', 'T', '1'), // 'DXT1'
    DDS_DXT3 = MAGIC('D', 'X', 'T', '3'), // 'DXT3'
    DDS_DXT5 = MAGIC('D', 'X', 'T', '5'), // 'DXT5'
    DDS_ATI1 = MAGIC('A', 'T', 'I', '1'), // 'ATI1'
    DDS_DX10 = MAGIC('D', 'X', '1', '0'), // 'DX10'
    DXT1_BLOCK_SIZE = 0x8,
    DXT3_BLOCK_SIZE = 0x10,
    DXT5_BLOCK_SIZE = 0x10
}dds_bc_format;

// HEAVILY abbreviated list from MSDN:
// https://learn.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format
typedef enum {
    DXGI_FORMAT_BC1_UNORM_SRGB = 71,
    DXGI_FORMAT_BC2_UNORM_SRGB = 75,
    DXGI_FORMAT_BC3_UNORM_SRGB = 78,
}dxgi_formats;

typedef struct dds_pixel_format {
    u32 size; // Must be 32 (0x20)
    u32 flags;
    u32 format_char_code; // See dwFourCC here: https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-pixelformat
    u32 bits_per_pixel;
    u32 red_bitmask;
    u32 green_bitmask;
    u32 blue_bitmask;
    u32 alpha_bitmask;
}dds_pixel_format;
static_assert(sizeof(dds_pixel_format) == 0x20, "dds_pixel_format size is wrong!");

// Extended DDS header:
// https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-header-dxt10
typedef struct {
    u32 dxgi_format;
    u32 resource_dimension; // Uses dds_resource_dimension enum
    u32 misc_flags; // Set to FLAG_2D_TEXTURECUBE to indicate it's a cubemap.
    u32 array_size; // # of textures, or # of cubemaps (6 textures each)
    u32 misc_flags2; // New alpha settings
}dx10_extended_format;

typedef enum {
    DIMENSION_1D = 2,
    DIMENSION_2D = 3,
    DIMENSION_3D = 4
}dds_resource_dimension;

enum {
    DDS_BEGIN = MAGIC('D', 'D', 'S', ' '), // 'DDS '
    FLAG_2D_TEXTURECUBE = 4,
};

typedef struct dds_header {
    u32 identifier;   // DDS_BEGIN as defined above. aka "file magic" / "magic number".
    u32 size;         // Must be 124 (0x7C)
    u32 flags;
    u32 height;
    u32 width;
    // For uncompressed textures, the size of the first row of pixels.
    // For compressed textures, the size of the entire texture at mip level 0.
    u32 pitch_or_linear_size;
    u32 depth;
    u32 mipmap_count;
    u32 reserved[11]; // Unused
    dds_pixel_format pixel_format;
    u32 caps;         // Flags for complexity of the surface
    u32 caps2;        // Always 0 because we don't use cubemaps or volumes
    u32 caps3;        // Unused
    u32 caps4;        // Unused
    u32 reserved2;    // Unused
}dds_header;
// The header itself is 0x7C (as MS specifies), but our struct includes the DDS
// magic as part of the header, adding 4 bytes.
static_assert(sizeof(dds_header) == 0x7C + 4, "dds_header size is wrong!");

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
                fallthrough;
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
            fallthrough;
        case 3:
            header.pixel_format.blue_bitmask = channel_mask << (2 * bits_per_channel);
            fallthrough;
        case 2:
            header.pixel_format.green_bitmask = channel_mask << (1 * bits_per_channel);
            fallthrough;
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
            fallthrough;
        }
        case 3:
            header.pixel_format.flags |= DDPF_RGB;
            break;
        case 2:
            // The only way to get a 2-channel image is to use the alpha & red channels
            header.pixel_format.alpha_bitmask = header.pixel_format.green_bitmask;
            header.pixel_format.green_bitmask = 0;
            header.pixel_format.flags = DDPF_ALPHAPIXELS;
            fallthrough;
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


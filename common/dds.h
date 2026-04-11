#pragma once
#include "util.h"
EXTERN_C_BEGIN

#include <assert.h>
#include "int.h"
#include "file.h"

// Most of these structures come from this MSDN page:
// https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-header


typedef enum {
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

typedef enum {
    DDSCAPS_COMPLEX = 0x00000008,
    DDSCAPS_MIPMAP  = 0x00400000,
    DDSCAPS_TEXTURE = 0x00001000
}dds_caps_flags;

// See dwFlags table on MSDN:
// https://learn.microsoft.com/en-us/windows/win32/direct3ddds/dds-pixelformat
typedef enum {
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
    DDS_FLOAT = 114,
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
    DXGI_FORMAT_BC4_UNORM = 79,
    DXGI_FORMAT_B5G6R5_UNORM = 85,
    DXGI_FORMAT_B5G5R5A1_UNORM = 86,
    DXGI_FORMAT_B4G4R4A4_UNORM = 115,

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

EXTERN_C_END

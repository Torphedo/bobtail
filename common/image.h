#pragma once
#include "util.h"
EXTERN_C_BEGIN
/// @file image.h
/// @brief Utilities for handling raw and formatted image data

#include <stdbool.h>
#include "int.h"

/// The compressed image format of a texture.
typedef enum {
    /// RGBA at 4 bits per pixel (8 bytes per 4x4 block), aka BC1
    DXT1,
    /// RGBA using DXT1/BC1 for the RGB, and uncompressed 4-bit alpha (uncommon, aka BC2)
    DXT3,
    /// RGBA using DXT1/BC1 for the RGB, and BC4 for the alpha (aka BC3)
    DXT5,
    /// Greyscale at 4 bits per pixel (8 bytes per 4x4 block)
    BC4,

    /// These aren't really compressed, but the channels are less than 1 byte
    /// each and GIMP won't load handcrafted files with the correct channel masks.
    DDS_FORMAT_BGR_565, // 5 bits for blue/red, 6 bits for green
    DDS_FORMAT_BGRA_5551, // 5 bits per channel, 1 bit alpha
    DDS_FORMAT_BGRA_4444, // 4 bits per channel

    /// This is considered compressed
    DDS_FORMAT_FLOAT,

    /// Not a real enum value. Increment then modulo by this to cycle through formats
    DXT_ENUM_MAX,
} img_fmt_compressed;

enum {
    /// Width/height (in pixels) of a compressed texture block
    COMPRESSED_BLK_DIM = 4,
};

// TODO: The texture structure is effectively just a simplified DDS header. We
// should replace image saving/loading code with 2 simple functions to convert
// texture struct <-> DDS header. The caller can easily write a DDS file from
// that, and the library code becomes decoupled from the file I/O.
// (extended headers might be tough, though).
typedef struct {
    /// Raw image data
    u8* data;
    u16 width; // u16 is plenty for any image
    u16 height;
    bool use_mipmaps;

    bool compressed;
    img_fmt_compressed fmt;

    /// @brief The number of bytes per channel (Only used if uncompressed, see detailed description)
    u8 unit_size;
    /// @brief Number of uncompressed color channels
    u8 channels;

    // Whether this is a cubemap, with a texture for each of the 6 directions
    bool cubemap;
    // Alignment of each of the 6 cubemap textures
    u16 cubemap_alignment;
} texture;

/// Round image dimensions down to some value
void img_snap(texture* img, u32 size);

/// @brief Find the total number of pixels in a texture with full mipmaps
/// @param width The texture width
/// @param height The texture height
/// @param compressed If the texture is block-compressed
/// @return The number of pixels
u64 pixel_count_max_mips(u32 width, u32 height, bool compressed);

/// Save an image to a DDS file
void img_write(texture img, const char* path);

/// @brief Parse a DDS in memory
/// @param buf The DDS data
/// @param buf_size The size of the DDS data
/// @param newAlloc Whether to copy the texture to a newly allocated buffer, or
/// just store a pointer to the start of the texture in the DDS buffer. If true,
/// the data field can be freed directly. Otherwise, you'll have to keep track
/// of the DDS buffer's pointer separately and free that.
/// @return The loaded texture
texture image_load_memory(const void* buf, u32 buf_size, bool newAlloc);

/// @brief Load a DDS from a file
///
/// You have to free the data field yourself later on.
/// @param path The file path of the DDS
texture image_load_file(const char* path);

/// Load a DDS from disk into a fixed size buffer
texture image_buf_load(const char* filename, u8* img_buf, u32 buf_size);

/// @brief Find out the size of the image data in a DDS file in memory
u32 image_required_size_memory(const void* buf, u32 buf_size);

/// @brief Find out the size of the image data in a DDS file
u32 image_required_size_file(const char* path);

EXTERN_C_END

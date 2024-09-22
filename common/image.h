#ifndef IMAGE_H
#define IMAGE_H
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
    /// Not a real enum value. Increment then modulo by this to cycle through formats
    DXT_ENUM_MAX,
}img_fmt_compressed;

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
    /// Number of mipmaps
    u16 mip_level;

    bool compressed;
    img_fmt_compressed fmt;

    /// @brief The size of each channel (Only used if uncompressed, see detailed description)
    ///
    /// 0 = u8, 1 = u16, 2 = u32
    u8 unit_size;
    /// @brief Number of uncompressed color channels
    u8 channels;
}texture;

/// Round image dimensions down to some value
void img_snap(texture* img, u32 size);

/// Save an image to a DDS file
void img_write(texture img, const char* path);

/// Load a DDS from disk
texture image_buf_load(const char* filename, u8* img_buf, u32 buf_size);

#endif // #ifndef IMAGE_H


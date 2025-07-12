#ifndef CRC32_H
#define CRC32_H
#ifdef __cplusplus
extern "C" {
#endif

#include "int.h"

/// @brief Compute the CRC-32 hash of a buffer
///
/// WARNING: There are several incompatible variants of 32-bit CRCs!
/// This function is for "CRC-32", as used in Ethernet, PKZIP, etc.
/// The polynomial is 0xEDB88320 or 0x04C11DB7.
/// (these are the same value, one with the bits in reverse order)
/// @param buf The data to be hashed
/// @param len The size of the data
/// @return CRC-32 hash
u32 crc32buf(const u8* buf, u32 len);

/// @brief Compute the CRC-32C hash of a buffer
///
/// WARNING: There are several incompatible variants of 32-bit CRCs! This
/// This function is for "CRC-32C", as used in Ethernet, PKZIP, etc.
/// The polynomial is 0x1EDC6F41 or 0x82F63B78.
/// (these are the same value, one with the bits in reverse order)
/// @param buf The data to be hashed
/// @param len The size of the data
/// @return CRC-32 hash
u32 crc32c(const u8* buf, u32 len);

#define crc32fast crc32c

#ifdef __cplusplus
}
#endif
#endif // #ifndef CRC32_H

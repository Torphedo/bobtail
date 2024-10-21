#ifndef CRC32_H
#define CRC32_H
#include "int.h"

/// @brief Compute the CRC32 hash of any piece of data
/// @param buf The data to be hashed
/// @param len The size of the data
/// @return CRC32 hash
u32 crc32buf(const u8* buf, u32 len);

#endif // #ifndef CRC32_H

#ifndef CRC32_H
#define CRC32_H
#ifdef __cplusplus
extern "C" {
#endif

#include "int.h"

/// @brief Compute the CRC32 hash of any piece of data
/// @param buf The data to be hashed
/// @param len The size of the data
/// @return CRC32 hash
u32 crc32buf(const u8* buf, u32 len);

#ifdef __cplusplus
}
#endif
#endif // #ifndef CRC32_H
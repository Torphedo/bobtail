#pragma once
#include "util.h"
EXTERN_C_BEGIN

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
u32 crc32buf(const u8* buf, u64 len);

/// @brief Compute the CRC-32C hash of a buffer
///
/// WARNING: There are several incompatible variants of 32-bit CRCs! This
/// This function is for "CRC-32C", as used in iSCSI, btrfs, and ext4
/// The polynomial is 0x1EDC6F41 or 0x82F63B78.
/// (these are the same value, one with the bits in reverse order)
/// If possible, SSE hardware acceleration will be used.
/// @param buf The data to be hashed
/// @param len The size of the data
/// @return CRC-32C hash
u32 crc32c(const u8* buf, u64 len);

// SSE-only implementation of CRC32C, exposed only for unit testing
u32 sse_crc32c(const u8* buf, u64 len);

// Software-only implementation of CRC32C. Please don't call directly, the
// generic version will do SSE for you if possible.
u32 software_crc32c(const u8* buf, u64 len);

// Always calls the fastest available variant of CRC32.
#define crc32fast crc32c

EXTERN_C_END
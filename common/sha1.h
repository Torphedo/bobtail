#ifndef SHA1_H
#define SHA1_H
/// @file sha1.h
/// @brief SHA-1 hashing implementation
///
/// This file is derived from the public domain C code in RFC 3171, and is also
/// released into the public domain.

#include <stdbool.h>
#include "int.h"

enum {
    /// Size in bytes of a SHA-1 digest
    SHA1_HASH_SIZE = 20,
};

/// A simple struct allowing SHA-1 digests to be passed between functions
typedef struct {
    u8 bytes[SHA1_HASH_SIZE];
}sha1_digest;

/// Calculate SHA-1 digest of any buffer
sha1_digest SHA1_buf(u8* buf, u64 len);

/// Compare 2 SHA-1 digests (memcmp() wrapper)
bool SHA1_equal(sha1_digest x, sha1_digest y);

/// Compare a SHA-1 digest to an all-zero digest
bool SHA1_blank(sha1_digest x);

/// Print a SHA-1 digest in uppercase hexidecimal format (no newline)
void SHA1_print(sha1_digest x);
#endif // SHA1_H

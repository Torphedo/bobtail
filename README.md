# Bobtail
This project is essentially my own mini standard library for C11. The brittle
parts (data structures) are unit-tested. The codebase is mostly C99, I only
really use `static_assert` from C11.

There are doxygen docs for the entire library, but if you're just browsing this
repo, here's a quick overview:

- File Utilities [`common/file.h`]
  - Simple wrappers around `stat` and `stdio.h`, useful for when you want
  something simple done with less boilerplate.
  - Read entire file [into a new or existing buffer]
  - File size at a path
  - "is a file/directory" checks


- Endian Handling [`common/endian.h`]
  - Generic macros to flip the endian of any value (in-place), big vs. little
  endian check function. Endian swaps compile down to a single instruction on
  many compilers, and the endian check is usually optimized out completely.


- Cross-Platform Virtual Memory Utilities [`common/vmem.h`]
  - Virtual memory helper API for Windows, Linux/Unix/MacOS, and Nintendo Switch.
    A different implementation is compiled depending on the platform, but the
    API's behaviour is almost identical on all platforms.
  - Reserve/commit API similar to `VirtualAlloc`
  - "[Magic](https://fgiesen.wordpress.com/2012/07/21/the-magic-ring-buffer/)"
    Ring Buffer that looks and acts like one, but is laid out in memory like a
    normal linear buffer. It can be seamlessly passed into code that expects a
    linear buffer, and the wrap-around will happen with zero special handling
    code on our end. Each "ring segment" has a minimum size of 64K (4K is possible
    on POSIX, but the API uses 64K for consistency with Windows). Not available on
    Nintendo Switch.


- "Virtual Files" [`common/vfile.h`]
  - `stdio`-style API that acts on buffers instead of files.
  - Has none of the alignment/size restrictions of anonymous memory-mapped files


- DDS Image Load/Save [`common/image.h`]
  - Provides a simplified image structure that can express most common image
    formats found in DDS files
  - This is DDS-only, so it's mostly good for game textures and graphics APIs.


- Containers [`common/buffer.h`, `common/list.h`, `common/queue.h`, & `common/hashmap.h`]
  - Exactly what they sound like. Removing items is constant-time for the list
    and queue.
  - The hashmap has a C++ wrapper class in `common/cpp/hashmap.hxx`, which is
    header-only so that the main library doesn't require a C++ compiler.


- Reasonable Platform Macros [`common/platform.h`]
  - I got tired of dealing with all the inconsistent, underscore-filled platform
  macros everywhere. These are really just re-skins, but named reasonable things
  you can remember:
    - `PLATFORM_WINDOWS`
    - `PLATFORM_UNIX`
    - `PLATFORM_LINUX`
    - `PLATFORM_BSD`
    - `PLATFORM_APPLE`
    - `PLATFORM_SWITCH`


- UTF-8 [`common/utf8.h`]
  - Unicode codepoint <-> UTF-8 translation


- OpenGL Helpers [`common/shader.h` & `common/gl_debug.h`]
  - These all assume GLAD as their OpenGL header
  - Shader compilation wrappers that automatically print errors
  - Debug Error Handler
    - Simple color-coded debug logger for OpenGL debug contexts
    - Assumes GLFW is present only to check for the debug context extension)


- Hashing [`common/crc32.h` & `common/sha1.h`]
  - These are just public domain hash function implementations I re-use often.
    - CRC32 by Gary S. Brown, 1986
    - CRC32-C has a SIMD implementation with a fallback software implementation
    - SHA-1 (slightly) adapted from RFC 3171


- Misc. Utilities [`common/int.h`]
   - Shorthands for `stdint.h` types, and random functions like `MAX`,
   `ARRAY_SIZE`, and bitmask helpers.

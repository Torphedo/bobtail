# Bobtail
This project is essentially my own mini standard library for C11. I ended up
needing a lot of the same helper functions in most of my projects, so I finally
stopped copy-pasting the files between projects and made it a separate repo.
Not everything is unit-tested, but some of the more brittle parts are.

There are doxygen docs for the entire library, but if you're just browsing this
repo, here's a quick overview:

- File Utilities [common/file.h]
  - Simple wrappers around `stat` and `stdio.h`, useful for when you want
  something simple done with less boilerplate.
  - Read entire file [into a new or existing buffer]
  - File size at a path
  - "is a file/directory" checks


- Endian Handling [common/endian.h]
  - Generic macros to flip the endian of any value (in-place), big vs. little
  endian check function. Endian swaps compile down to a single instruction on
  many compilers, and the endian check is usually optimized out completely.


- Cross-Platform Virtual Memory Utilities [common/vmem.h]
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


- "Virtual File" [common/vfile.h]
  - `stdio`-style API that acts on buffers of memory instead of files.
  - Has none of the alignment/size restrictions of anonymous memory-mapped files


- Image/Texture Processing [common/image.h]
  - Fairly simple API to handle image formats commonly used in DDS files. It's
    essentially a simpler DDS structure that can express *most* image formats
    expressable in DDS.
  - Geared towards game texture formats used in graphics APIs, not everyday
    formats like PNG or JPEG.


- Dynamic List/Queue [common/list.h & common/queue.h]
  - Exactly what they sound like. Pointers to individual elements may be
  invalidated, and removing items is constant-time.


- Reasonable Platform Macros [common/platform.h]
  - I got tired of dealing with all the inconsistent, underscore-filled platform
  macros everywhere. These are really just re-skins, but named reasonable things
  you can remember:
    - `PLATFORM_WINDOWS`
    - `PLATFORM_UNIX`
    - `PLATFORM_LINUX`
    - `PLATFORM_BSD`
    - `PLATFORM_APPLE`
    - `PLATFORM_SWITCH`


- UTF-8 [common/utf8.h]
  - Unicode codepoint <-> UTF-8 translation


- OpenGL Helpers [common/shader.h & common/gl_debug.h & common/model.h]
  - These all assume GLAD as their OpenGL header
  - Shader Compilation
    - Compile normal (vertex & fragment) shaders, check for errors, and print
   error messages
  - Debug Error Handler
    - Simple color-coded debug logger for OpenGL debug contexts
    - Assumes GLFW is present (only to check for the extension)
  - Models & Primitives
    - `.obj` loader for simple meshes with only position & vertex color
    - Pre-defined cube & quads
    - Not generic by any means and specific to my needs, but probably a good
    starting point if you know at least a little about vertex layout in OpenGL


- Hashing [common/crc32.h & common/sha1.h]
  - These are just public domain hash function implementations I re-use often.


- Misc. Utilities [common/int.h]
   - Shorthands for `stdint.h` types, and random functions like `MAX`,
   `ARRAY_SIZE`, and bitmask helpers.

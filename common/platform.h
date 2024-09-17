#ifndef PLATFORM_H
#define PLATFORM_H
/// @file platform.h
/// @brief Simple definitions to find out the target platform
///
/// Possible platform definitions are:<br>
///
/// PLATFORM_APPLE<br>
/// PLATFORM_UNIX<br>
/// PLATFORM_LINUX<br>
/// PLATFORM_BSD<br>
/// PLATFORM_POSIX<br>
/// PLATFORM_WINDOWS<br>
/// PLATFORM_SWITCH (Nintendo Switch)<br>
///
/// These will all be a constant 1 if defined.

#if (defined(__APPLE__) && defined(__MACH__))
    #define PLATFORM_APPLE 1
#endif

#if defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__))
    #define PLATFORM_UNIX 1

    #include <sys/param.h>
    #ifdef BSD
        #define PLATFORM_BSD 1 
    #endif

    #if defined(__linux) || defined(__linux__)
        #define PLATFORM_LINUX 1
    #endif

    #include <unistd.h>
    #if defined(_POSIX_VERSION)
        #define PLATFORM_POSIX 1
    #endif
#elif defined (WIN32) || defined(_WIN32)
    #define PLATFORM_WINDOWS 1
#endif

#ifdef __SWITCH__
    #define PLATFORM_SWITCH 1
#endif


#if defined(PLATFORM_WINDOWS)
    #define PLATFORM_DIRSEP ('\\')
#else
    #define PLATFORM_DIRSEP ('/')
#endif

#endif // #ifndef PLATFORM_H

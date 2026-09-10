/*
  SPDX-FileCopyrightText: 2026 Vovoid Media Technologies AB
  Author: Jonatan Wallmander <jonatan@vovoid.com>

  SPDX-License-Identifier: MIT
*/
#pragma once

#define PLATFORM_WINDOWS          0
#define PLATFORM_LINUX          1
#define PLATFORM_WM6                  3
#define PLATFORM_ANDROID          4
#define PLATFORM_SOMETHING_ELSE    1000

#define PLATFORM_FAMILY_WINDOWS 0
#define PLATFORM_FAMILY_UNIX 1
#define PLATFORM_FAMILY_OTHER 2

#define COMPILER_GCC 1
#define COMPILER_MINGW 2
#define COMPILER_VISUAL_STUDIO 4

#ifndef PLATFORM
#if defined(WIN32) || defined(WIN64) || defined(_WIN32) || defined(_WIN64)

#include <windows.h>
#ifdef __GNUC__
#define COMPILER COMPILER_MINGW + COMPILER_GCC
#endif
#ifdef _MSC_VER
#define COMPILER COMPILER_VISUAL_STUDIO
#endif
#define PLATFORM                                        PLATFORM_WINDOWS
#define PLATFORM_NAME                                   "Windows"
#define PLATFORM_FAMILY                                 PLATFORM_FAMILY_WINDOWS
#define PLATFORM_SHARED_FILES                           vsx_string<>("artiste\\")
#define PLATFORM_DLL_SUFFIX                             ".dll"
#define DIRECTORY_SEPARATOR                             "\\"
#define DIRECTORY_SEPARATOR_CHAR                             '\\'
#define VSXU_PLUGIN_LOCATION                           vsx_string<>("plugins/")

#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__CYGWIN__) || defined(VZX_ASTREE)
#define COMPILER COMPILER_GCC
        #define PLATFORM                                        PLATFORM_LINUX
        #define PLATFORM_NAME                                   "GNU / Linux"
        #define PLATFORM_FAMILY                                 PLATFORM_FAMILY_UNIX
        #define PLATFORM_SHARED_FILES                           vsx_string<>(get_exec_path().c_str()) + "/../share/vsxu/"
        #define VSXU_PLUGIN_LOCATION                           vsx_string<>(get_exec_path().c_str()) + "/../lib/vsxu/plugins"
        #define PLATFORM_DLL_SUFFIX                             ".so"
        #define DIRECTORY_SEPARATOR                             "/"
        #define DIRECTORY_SEPARATOR_CHAR                        '/'
    #else

        #define PLATFORM                                        PLATFORM_SOMETHING_ELSE
        #define PLATFORM_NAME                                   "Something Else"
        #define PLATFORM_FAMILY                                 PLATFORM_FAMILY_OTHER
        #define PLATFORM_SHARED_FILES                           ""
        #define DIRECTORY_SEPARATOR                             "/"
        #define DIRECTORY_SEPARATOR_CHAR                        '/'
        #define VSXU_PLUGIN_LOCATION                           vsx_string<>("plugins/")
#endif

// aligned malloc
#ifdef VZX_ASTREE
  #define vsx_aligned_malloc(n) __astree_malloc(0, n, "malloc")
  #define vsx_aligned_realloc(pointer, n) __astree_malloc(pointer, n, "malloc")
  #define vsx_aligned_free(pointer) __astree_malloc(pointer, 0, "malloc")
#else
  // MSVC
  #if COMPILER == COMPILER_VISUAL_STUDIO
  #include <malloc.h>
  #ifdef VMONO_ENABLE_MIMALLOC
  #define vsx_aligned_malloc(n) mi_malloc(n)
  #define vsx_aligned_realloc(pointer, n) mi_realloc(pointer, n)
  #define vsx_aligned_free(pointer) mi_free(pointer)
  #else
  #define vsx_aligned_malloc(n) _aligned_malloc(n, 64)
  #define vsx_aligned_realloc(pointer, n) _aligned_realloc(pointer, n, 64)
  #define vsx_aligned_free(pointer) _aligned_free(pointer)
  #endif // VMONO_ENABLE_MIMALLOC
  #endif // COMPILER_VISUAL_STUDIO

  // Linux
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <stdio.h>
  #include <stdlib.h>
  #include <unistd.h>
  #include <libgen.h>
  #include <string>
  #include <cstring>
  inline void* vsx_aligned_malloc(size_t n)
  {
    void* r;
    int f = posix_memalign(&r, 64, n);
    (void)f;
    return r;
  }
  #define vsx_aligned_realloc(pointer, n) realloc(pointer, n)
  #define vsx_aligned_free(pointer) free(pointer)
  #endif // PLATFORM_FAMILY_UNIX
#endif // VZX_ASTREE
#endif

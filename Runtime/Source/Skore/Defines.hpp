// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef unsigned long int  ul32;

typedef signed char      i8;
typedef signed short     i16;
typedef signed int       i32;
typedef signed long long i64;

typedef float  f32;
typedef double f64;

#if _WIN64
#   define SK_API __declspec(dllexport)
#   define SK_WIN  1
#   define SK_PATH_SEPARATOR '\\'
#   define SK_SHARED_EXT ".dll"
#   define SK_FINLINE __forceinline
#elif __linux__
#   define SK_API   __attribute__ ((visibility ("default")))
#   define SK_PATH_SEPARATOR '/'
#   define SK_UNIX
#   define SK_LINUX
#   define SK_SHARED_EXT ".so"
#   define SK_FINLINE inline
#elif __APPLE__
#   define SK_API
#   define SK_PATH_SEPARATOR '/'
#   define SK_UNIX
#   define SK_SHARED_EXT ".dylib"
#   define SK_FINLINE static inline
#else
#   error "Unknown Apple platform"
#endif



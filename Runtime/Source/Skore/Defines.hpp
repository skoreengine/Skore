// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#pragma once

typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef unsigned long int  ul32;
typedef signed char        i8;
typedef signed short       i16;
typedef signed int         i32;
typedef signed long long   i64;

typedef float  f32;
typedef double f64;

typedef void      * CPtr;
typedef const void* ConstCPtr;
typedef u64 TypeID;

typedef decltype(sizeof(0)) usize;

#define SK_STRING_BUFFER_SIZE 18
#define SK_REPO_PAGE_SIZE 4096

#if _WIN64
#   define SK_API __declspec(dllexport)
#   define SK_WIN  1
#   define SK_DESKTOP  1
#   define SK_PATH_SEPARATOR '\\'
#   define SK_SHARED_EXT ".dll"
#   define SK_FINLINE __forceinline
#elif __linux__
#   define SK_API   __attribute__ ((visibility ("default")))
#   define SK_DESKTOP  1
#   define SK_PATH_SEPARATOR '/'
#   define SK_UNIX
#   define SK_LINUX
#   define SK_SHARED_EXT ".so"
#   define SK_FINLINE inline
#   define SK_TEXT(s) s
#elif __APPLE__
#   define SK_API
#   define SK_PATH_SEPARATOR '/'
#   define SK_UNIX
#   define SK_APPLE
#   define SK_SHARED_EXT ".dylib"
#   define SK_FINLINE inline
	#if TARGET_IPHONE_SIMULATOR
    // iOS, tvOS, or watchOS Simulator
    #elif TARGET_OS_MACCATALYST
    // Mac's Catalyst (ports iOS API into Mac, like UIKit).
    #elif TARGET_OS_IPHONE
		#define SK_MOBILE 1
    #elif TARGET_OS_MAC
    // Other kinds of Apple platforms
#       define SK_DESKTOP
    #else
    #   error "Unknown Apple platform"
	#endif
#else
#   error "Unknown Apple platform"
#endif

#ifndef SK_PRETTY_FUNCTION
#if defined _MSC_VER
#   define SK_PRETTY_FUNCTION __FUNCSIG__
#   define SK_PRETTY_FUNCTION_PREFIX '<'
#   define SK_PRETTY_FUNCTION_SUFFIX '>'
#elif defined __clang__ || defined __GNUC__
#   define SK_PRETTY_FUNCTION __PRETTY_FUNCTION__
#   define SK_PRETTY_FUNCTION_PREFIX '='
#   define SK_PRETTY_FUNCTION_SUFFIX ']'
#endif
#endif

#define SK_HANDLER(StructName) struct StructName { \
    CPtr Handler;                                  \
 operator bool() const {return Handler != nullptr; }          \
 bool operator==(const StructName& b) const { return this->Handler == b.Handler; } \
 bool operator!=(const StructName& b) const { return this->Handler != b.Handler; } \
}


#if defined _MSC_VER
//unsigned int MAX
#   define U8_MAX           0xffui8
#   define U16_MAX          0xffffui16
#   define U32_MAX          0xffffffffui32
#   define U64_MAX          0xffffffffffffffffui64

//signed int MIN
#   define I8_MIN           (-127i8 - 1)
#   define I16_MIN          (-32767i16 - 1)
#   define I32_MIN          (-2147483647i32 - 1)
#   define I64_MIN          (-9223372036854775807i64 - 1)

//signed int MAX
#   define I8_MAX           127i8
#   define I16_MAX          32767i16
#   define I32_MAX          2147483647i32
#   define I64_MAX          9223372036854775807i64

#   define F32_MAX          3.402823466e+38F
#   define F64_MAX          1.7976931348623158e+308

#   define F32_MIN          1.175494351e-38F
#   define F64_MIN          2.2250738585072014e-308

#   define F32_LOW          (-(F32_MAX))
#   define F64_LOW          (-(F64_MAX))

#elif defined __GNUC__
# define I8_MIN		    (-128)
# define I16_MIN		(-32767-1)
# define I32_MIN		(-2147483647-1)
# define I64_MIN	    INT64_MIN

# define I8_MAX		    (127)
# define I16_MAX		(32767)
# define I32_MAX		(2147483647)
# define I64_MAX		INT64_MAX

/* Maximum of unsigned integral types.  */
# define U8_MAX		    (255)
# define U16_MAX		(65535)
# define U32_MAX		(4294967295U)
# define U64_MAX		18446744073709551615UL

# define F32_MAX        __FLT_MAX__
# define F64_MAX        __DBL_MAX__

# define F32_MIN        __FLT_MIN__
# define F64_MIN        __DBL_MIN__

# define F32_LOW         (-(F32_MAX))
# define F64_LOW         (-(F64_MAX))
#endif


#ifdef NDEBUG
#  define SK_ASSERT(condition, message) ((void)0)
#else
#  include <cassert>
#  define SK_ASSERT(condition, message) assert(condition && message)
#  define SK_DEBUG
#endif


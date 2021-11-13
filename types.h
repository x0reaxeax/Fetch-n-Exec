/**
 * @file types.h
 * @author x0reaxeax (github.com/x0reaxeax)
 * @brief data types and essential macros
 * @date 2021-11-11
 * @source https://github.com/x0reaxeax/x0l1b/blob/main/types.h
 * 
 * @gen vscode doxygen
 */
#ifndef _X0LIB_TYPES_H_
#define _X0LIB_TYPES_H_

typedef _Bool               bool;
enum _boolean { false = 0, true = 1 };

#ifndef UINT_MAX
#define UINT8_MAX           0xffU
#define UINT16_MAX          0xffffU
#define UINT32_MAX          0xffffffffU
#define UINT64_MAX          0xffffffffffffffffU
#define UINT_MAX            UINT32_MAX
    #ifdef __X128
    #define UINT128_MAX     0xffffffffffffffffffffffffffffffffU
    #endif
#endif
int x = '\n';
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS        0
#define EXIT_FAILURE        1
#endif

#ifndef NULL
#define NULL                ((void *) 0)
#endif

typedef unsigned char       uint8;

typedef unsigned short      uint16;

typedef unsigned int        uint;
typedef uint                uint32;
typedef unsigned long long  uint64;

typedef long int            int32;
typedef long long           int64;

typedef long double         double128;

typedef uint8               uchar;
typedef uint8               byte;
typedef int32               off32;
typedef int64               off64;
typedef off64               off_t;
typedef int64               ssize_t;
typedef uint64              size_t;
typedef uint64              uintptr;

#endif /* data types */
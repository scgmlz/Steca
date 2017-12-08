#ifndef CAR_DATATYPES_H
#define CAR_DATATYPES_H

#undef PLATFORM
#undef BIG_ENDIAN
#undef LITTLE_ENDIAN
#undef NATIVE_DATAWIDTH

#ifdef __cplusplus
extern "C" {
#endif

/*
 * INTEGER: int8, uint8, int16, uint16, int32, uint32, int64, uint64
 */

/* x64/x86_64: 64 bit, little endian */
#if !defined(PLATFORM) &&                                                                          \
    (defined(__amd64) || defined(__amd64__) || defined(__x86_64) || defined(__x86_64__) || defined(_WIN64))
#define LITTLE_ENDIAN 1
#define PLATFORM "x86_64"
#define NATIVE_DATAWIDTH 64
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long int64;
typedef unsigned long uint64;
#endif

/* x86: 32 bit, little endian */
#if (!defined(PLATFORM)) && (defined(i386) || defined(__x86__) || defined(_WIN32))
#define LITTLE_ENDIAN 1
#define PLATFORM "x86"
#define NATIVE_DATAWIDTH 32
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
#endif

/* PowerPC64: 64 bit, big endian */
#if (!defined(PLATFORM)) && (defined(__PPC64__) || defined(__powerpc64__))
#define BIG_ENDIAN 1
#define PLATFORM "powerpc64"
#define NATIVE_DATAWIDTH 64
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long int64;
typedef unsigned long uint64;
#endif

/* PowerPC: 32 bit, big endian */
#if (!defined(PLATFORM)) && (defined(__PPC__) || defined(__powerpc__) || defined(__ppc__))
#define BIG_ENDIAN 1
#define PLATFORM "powerpc"
#define NATIVE_DATAWIDTH 32
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
#endif

/* alpha: 64 bit, little endian */
#if (!defined(PLATFORM)) && defined(__alpha) && defined(__osf__) && defined(__unix__)
#define LITTLE_ENDIAN 1
#define PLATFORM "alpha"
#define NATIVE_DATAWIDTH 64
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long int64;
typedef unsigned long uint64;
#endif

/* sparc: 32 bit, big endian */
#if (!defined(PLATFORM)) && defined(__sun) && defined(__sparc) && defined(__unix__)
#define BIG_ENDIAN 1
#define PLATFORM "sparc"
#define NATIVE_DATAWIDTH 32
typedef signed char int8;
typedef unsigned char uint8;
typedef signed short int16;
typedef unsigned short uint16;
typedef signed int int32;
typedef unsigned int uint32;
typedef signed long long int64;
typedef unsigned long long uint64;
#endif

#ifdef LITTLE_ENDIAN
#ifdef BIG_ENDIAN
#error "unknown byte order"
#endif
#else
#ifndef BIG_ENDIAN
#error "unknown byte order"
#endif
#endif

#ifndef BIG_ENDIAN
#if defined(__BIG_ENDIAN__)
#error "byte order mismatch"
#endif
#endif

#ifndef PLATFORM
#error "unknown system platform"
#endif

#ifndef NATIVE_DATAWIDTH
#error "unknown native system data width"
#endif

uint16 integer16_swap(uint16 x);
uint32 integer32_swap(uint32 x);
uint64 integer64_swap(uint64 x);
float ieee32_swap(float x);
double ieee64_swap(double x);

#ifdef LITTLE_ENDIAN
#define be_int16(x) integer16_swap(x)
#define be_int32(x) integer32_swap(x)
#define be_int64(x) integer64_swap(x)
#define be_ieee32(x) ieee32_swap(x)
#define be_ieee64(x) ieee64_swap(x)
#define le_int16(x) (x)
#define le_int32(x) (x)
#define le_int64(x) (x)
#define le_ieee32(x) (x)
#define le_ieee64(x) (x)
#endif

#ifdef BIG_ENDIAN
#define be_int16(x) (x)
#define be_int32(x) (x)
#define be_int64(x) (x)
#define be_ieee32(x) (x)
#define be_ieee64(x) (x)
#define le_int16(x) integer16_swap(x)
#define le_int32(x) integer32_swap(x)
#define le_int64(x) integer64_swap(x)
#define le_ieee32(x) ieee32_swap(x)
#define le_ieee64(x) ieee64_swap(x)
#endif

#ifdef __cplusplus
}
#endif

#endif // CAR_DATATYPES_H

/*
	This code is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This code is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.
*/

#ifdef FASTC_BASE_HAS_CPP11_TYPES
#include <cstdint>

typedef int8_t int8;
typedef uint8_t uint8;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

typedef char CHAR;

#else

// Windows?
#ifdef _MSC_VER

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int8 int8;
typedef unsigned __int8 uint8;

typedef unsigned __int64 uint64;
typedef __int64 int64;

#include <tchar.h>
typedef TCHAR CHAR;

// If not, assume GCC, or at least standard defines...
#else 

#include <stdint.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef char CHAR;

#endif // _MSC_VER

#endif // FASTC_BASE_HAS_CPP11_TYPES
// _TEX_COMP_TYPES_H_
namespace CPC
{
  // DXT compressor (scalar version).
	

  uint16 ColorTo565(const uint8* color);
  void EmitByte(uint8*& dest, uint8 b);
  void EmitWord(uint8*& dest, uint16 s);
  void EmitDoubleWord(uint8*& dest, uint32 i);

#if 0
  // DXT compressor (SSE2 version).
  void CompressImageDXT1SSE2(const uint8* inBuf, uint8* outBuf, uint32 width, uint32 height);
  void CompressImageDXT5SSE2(const uint8* inBuf, uint8* outBuf, uint32 width, uint32 height);
#endif
}

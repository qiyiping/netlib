/*
 * Copyright (c) 2012 Yiping Qi <qiyiping at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. Neither name of copyright holders nor the names of its contributors
 * may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _BINARY_IO_H_
#define _BINARY_IO_H_

#include "io.hpp"
#include <byteswap.h>
#include <arpa/inet.h>

namespace netlib {

// Check endian related macro
#ifndef LITTLE_ENDIAN
#ifndef BIG_ENDIAN
#error Both LITTLE_ENDIAN and BIG_ENDIAN are not defined
#endif
#endif

#ifndef BYTE_ORDER
#error BYTE_ORDER is not defined
#endif

// Define host/network byte order conversion for 64-bit integer
#if BYTE_ORDER == BIG_ENDIAN
#define ntohll(n) (n)
#define htonll(n) (n)
#elif BYTE_ORDER == LITTLE_ENDIAN
#define ntohll(n) bswap_64(n)
#define htonll(n) bswap_64(n)
#else
#error BYTE_ORDER is not LITTLE_ENDIAN nor BIG_ENDIAN
#endif

template <typename From, typename To>
static inline To bitwise_cast(From from) {
  NETLIB_STATIC_ASSERT((sizeof(From) == sizeof(To)), TypeSizeNotEqual);
  union {
    From f;
    To t;
  } u;
  u.f = from;
  return u.t;
}

class BinaryIO: public IOBase {
 public:
  int32_t ReadUInt8(uint8_t *ptr) {
    return ReadBytes(ptr, sizeof(*ptr));
  }

  int32_t ReadUInt16(uint16_t *ptr) {
    int32_t ret = ReadBytes(ptr, sizeof(*ptr));
    if (ret == sizeof(*ptr)) {
      *ptr = ntohs(*ptr);
    }
    return ret;
  }

  int32_t ReadUInt32(uint32_t *ptr) {
    int32_t ret = ReadBytes(ptr, sizeof(*ptr));
    if (ret == sizeof(*ptr)) {
      *ptr = ntohl(*ptr);
    }
    return ret;
  }

  int32_t ReadUInt64(uint64_t *ptr) {
    int32_t ret = ReadBytes(ptr, sizeof(*ptr));
    if (ret == sizeof(*ptr)) {
      *ptr = ntohll(*ptr);
    }
    return ret;
  }

  int32_t ReadInt8(int8_t *ptr) {
    return ReadBytes(ptr, sizeof(*ptr));
  }

  int32_t ReadInt16(int16_t *ptr) {
    int32_t ret = ReadBytes(ptr, sizeof(*ptr));
    if (ret == sizeof(*ptr)) {
      *ptr = ntohs(*ptr);
    }
    return ret;
  }

  int32_t ReadInt32(int32_t *ptr) {
    int32_t ret = ReadBytes(ptr, sizeof(*ptr));
    if (ret == sizeof(*ptr)) {
      *ptr = ntohl(*ptr);
    }
    return ret;
  }

  int32_t ReadInt64(int64_t *ptr) {
    int32_t ret = ReadBytes(ptr, sizeof(*ptr));
    if (ret == sizeof(*ptr)) {
      *ptr = ntohll(*ptr);
    }
    return ret;
  }

  int32_t WriteUInt8(uint8_t val) {
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteUInt16(uint16_t val) {
    val = htons(val);
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteUInt32(uint32_t val) {
    val = htonl(val);
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteUInt64(uint64_t val) {
    val = htonll(val);
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteInt8(int8_t val) {
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteInt16(int16_t val) {
    val = htons(val);
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteInt32(int32_t val) {
    val = htonl(val);
    return WriteBytes(&val, sizeof(val));
  }

  int32_t WriteInt64(int64_t val) {
    val = htonll(val);
    return WriteBytes(&val, sizeof(val));
  }

  int32_t ReadFloat(float *ptr) {
    uint32_t val;
    int32_t ret = ReadUInt32(&val);
    if (ret == sizeof(uint32_t)) {
      *ptr = bitwise_cast<uint32_t, float>(val);
    }
    return ret;
  }

  int32_t WriteFloat(float val) {
    uint32_t u = bitwise_cast<float, uint32_t>(val);
    return WriteUInt32(u);
  }

  int32_t ReadDouble(double *ptr) {
    uint64_t val;
    int32_t ret = ReadUInt64(&val);
    if (ret == sizeof(uint64_t)) {
      *ptr = bitwise_cast<uint64_t, double>(val);
    }
    return ret;
  }

  int32_t WriteDouble(double val) {
    uint64_t u = bitwise_cast<double, uint64_t>(val);
    return WriteUInt64(u);
  }

  virtual ~BinaryIO() {}
};
}

#endif /* _BINARY_IO_H_ */

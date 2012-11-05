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
#ifndef _BUFFER_IO_H_
#define _BUFFER_IO_H_
#include <string>
#include <algorithm>
#include "io.hpp"
namespace netlib {
class BufferIO: public IOBase {
 public:
  BufferIO(): pos_(0) {}
  BufferIO(const std::string &str): buffer_(str), pos_(0) {}

  const std::string &GetBuffer() const { return buffer_; }

  int32_t ReadBytes(void *buf, uint32_t size);
  int32_t WriteBytes(const void *buf, uint32_t size);

  int32_t ReadString (std::string *str);
  int32_t WriteString(const std::string &str);

  int32_t ReadUInt8(uint8_t *ptr) {
    return ReadNum(ptr);
  }
  int32_t ReadUInt16(uint16_t *ptr) {
    return ReadNum(ptr);
  }
  int32_t ReadUInt32(uint32_t *ptr) {
    return ReadNum(ptr);
  }
  int32_t ReadUInt64(uint64_t *ptr) {
    return ReadNum(ptr);
  }

  int32_t ReadInt8(int8_t *ptr) {
    return ReadNum(ptr);
  }
  int32_t ReadInt16(int16_t *ptr) {
    return ReadNum(ptr);
  }
  int32_t ReadInt32(int32_t *ptr) {
    return ReadNum(ptr);
  }
  int32_t ReadInt64(int64_t *ptr) {
    return ReadNum(ptr);
  }

  int32_t WriteUInt8(uint8_t val) {
    return WriteNum(val);
  }
  int32_t WriteUInt16(uint16_t val) {
    return WriteNum(val);
  }
  int32_t WriteUInt32(uint32_t val) {
    return WriteNum(val);
  }
  int32_t WriteUInt64(uint64_t val) {
    return WriteNum(val);
  }

  int32_t WriteInt8(int8_t val) {
    return WriteNum(val);
  }
  int32_t WriteInt16(int16_t val) {
    return WriteNum(val);
  }
  int32_t WriteInt32(int32_t val) {
    return WriteNum(val);
  }
  int32_t WriteInt64(int64_t val) {
    return WriteNum(val);
  }

  int32_t ReadFloat(float *ptr) {
    return ReadNum(ptr);
  }
  int32_t WriteFloat(float val) {
    return WriteNum(val);
  }
  int32_t ReadDouble(double *ptr) {
    return ReadNum(ptr);
  }
  int32_t WriteDouble(double val) {
    return WriteNum(val);
  }

 private:
  template<typename T>
  int32_t ReadNum(T *t) {
    uint8_t *buf = reinterpret_cast<uint8_t *>(t);
    if (ReadBytes(buf, sizeof(T)) != sizeof(T)) {
      return RETURN_ERR;
    }
#if (BYTE_ORDER == BIG_ENDIAN)
    std::reverse(buf, buf+sizeof(T));
#endif
    return RETURN_OK;
  }

  template<typename T>
  int32_t WriteNum(T t) {
    uint8_t *buf = reinterpret_cast<uint8_t *>(&t);
#if (BYTE_ORDER == BIG_ENDIAN)
    std::reverse(buf, buf+sizeof(T));
#endif
    if (WriteBytes(buf, sizeof(T)) != sizeof(T)) {
      return RETURN_ERR;
    }
    return RETURN_OK;
  }

  std::string buffer_;
  uint32_t pos_;

  DISALLOW_COPY_AND_ASSIGN(BufferIO);
};
}

#endif /* _BUFFER_IO_H_ */

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

#ifndef _HASH_H_
#define _HASH_H_

#include "config.hpp"
#include <string>

namespace netlib {
/**
 * Murmurhash: https://sites.google.com/site/murmurhash/
 *
 * @param key input key
 * @param len lenght of the input key in bytes
 * @param seed seed to initiate the internal state
 * @return a unsiged int 64 hash value
 */
uint64_t MurmurHash64(const void *key, int32_t len, uint32_t seed);


template <typename T> struct hash {};

template<>
struct hash<std::string> {
  std::size_t operator()(const std::string &s) {
    std::size_t h = 0;
    for (std::size_t i = 0; i < s.size(); ++i)
      h = (h<<5) - h + s[i];
    return h;
  }
};

inline std::size_t string_hash(const char *s) {
  std::size_t h = 0;
  for (; *s; ++s) {
    h = (h<<5) - h + *s;
  }
  return h;
}

template<>
struct hash<char *> {
  std::size_t operator()(char *s) {
    return string_hash(s);
  }
};

template<>
struct hash<const char *> {
  std::size_t operator()(const char *s) {
    return string_hash(s);
  }
};

template<>
struct hash<int8_t> {
  std::size_t operator()(int8_t i) { return i; }
};

template<>
struct hash<int16_t> {
  std::size_t operator()(int16_t i) { return i; }
};

template<>
struct hash<int32_t> {
  std::size_t operator()(int32_t i) { return i; }
};

template<>
struct hash<int64_t> {
  std::size_t operator()(int64_t i) { return i; }
};

template<>
struct hash<uint8_t> {
  std::size_t operator()(uint8_t i) { return i; }
};

template<>
struct hash<uint16_t> {
  std::size_t operator()(uint16_t i) { return i; }
};

template<>
struct hash<uint32_t> {
  std::size_t operator()(uint32_t i) { return i; }
};

template<>
struct hash<uint64_t> {
  std::size_t operator()(uint64_t i) { return i; }
};
}
#endif /* _HASH_H_ */

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

#ifndef _MINHASH_H_
#define _MINHASH_H_

#include <string>
#include <vector>
#include <limits>
#include <time.h>
#include <stdlib.h>
#include "config.hpp"
#include "hash.hpp"


namespace netlib {
// MinHash implementation
// http://en.wikipedia.org/wiki/MinHash

class MinHash {
 public:
  MinHash(int32_t size);
  template <typename InputIterator>
  std::vector<uint64_t> Hash(InputIterator first,
                             InputIterator last,
                             const void* (*data) (InputIterator),
                             int32_t (*length) (InputIterator));
 private:
  int32_t size_;
  std::vector<uint32_t> seeds_;
};

template <typename InputIterator>
std::vector<uint64_t> MinHash::Hash(InputIterator first,
                                    InputIterator last,
                                    const void* (*data) (InputIterator),
                                    int32_t (*length) (InputIterator)) {
  std::vector<uint64_t> ret(size_, kUInt64Max);
  while (first != last) {
    for (std::size_t i = 0; i < size_; ++i) {
      uint64_t val = MurmurHash64(data(first), length(first), seeds_[i]);
      if (ret[i] > val)
        ret[i] = val;
    }
    ++first;
  }
  return ret;
}

inline MinHash::MinHash(int32_t size): size_(size) {
  srand(time(NULL));
  for (std::size_t i = 0; i < size; ++i)
    seeds_.push_back(rand());
}

inline double JaccardSimilarity(const std::vector<uint64_t> &sig1,
                                const std::vector<uint64_t> &sig2) {
  if (sig1.size() != sig2.size())
    return 0.0;
  std::size_t c = 0;
  std::size_t s = sig1.size();
  for (std::size_t i = 0; i < s; ++i)
    if (sig1[i] == sig2[i] && sig1[i] != KUInt64Max)
      ++c;
  if (s == 0)
    return 0.0;
  return c/static_cast<double>(s);
}

}
#endif /* _MINHASH_H_ */

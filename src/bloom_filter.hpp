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

#ifndef _BLOOM_FILTER_H_
#define _BLOOM_FILTER_H_
#include <string>
#include <cmath>
#include <tr1/functional>
#include "config.hpp"
#include "hash.hpp"
#include <boost/scoped_array.hpp>

namespace netlib {
class BloomFilter {
 public:
  typedef std::tr1::function<uint64_t (const void *, int32_t, uint32_t)> HashFunction;

  BloomFilter(int64_t elements, double false_positive_prob,
              int64_t max_mem_usage = 4000, HashFunction func = MurmurHash64);
  BloomFilter(const std::string &path, HashFunction func = MurmurHash64);

  void Insert(const void *key, int32_t len);
  bool Exists(const void *key, int32_t len);
  void Insert(const std::string &key) {
    Insert(key.c_str(), key.length());
  }
  bool Exists(const std::string &key) {
    return Exists(key.c_str(), key.length());
  }

  int32_t WriteToFile(const std::string &path);
  int32_t ReadFromFile(const std::string &path);

  ~BloomFilter() {}

 private:
  static int64_t GetBitSize(int64_t elements, double false_positive_prob) {
    return static_cast<int64_t>(-elements*std::log(false_positive_prob)/(std::log(2.0)*std::log(2.0)));
  }
  static int64_t GetHashFunctionSize(int64_t bits, int64_t elements) {
    return static_cast<int64_t>(std::log(2.0)*bits/elements+0.5);
  }
  static double GetFalsePositiveProb(int64_t bits, int64_t elements) {
    return std::pow(0.5, GetHashFunctionSize(bits, elements));
  }

  HashFunction hash_func_;
  int64_t num_bits_;
  int64_t num_bytes_;
  int64_t num_seeds_;
  boost::scoped_array<uint8_t> data_;
  boost::scoped_array<uint32_t> seeds_;

  DISALLOW_COPY_AND_ASSIGN(BloomFilter);
};
}

#endif /* _BLOOM_FILTER_H_ */

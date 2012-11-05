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

#include "bloom_filter.hpp"
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glog/logging.h>

#include "file_io.hpp"

namespace netlib {

BloomFilter::BloomFilter(int64_t elements, double false_positive_prob,
                         int64_t max_mem_usage, HashFunction func): hash_func_(func) {
  CHECK_LT(false_positive_prob, 1.0) << "False positive probability should be less than 1.0";
  CHECK_GT(false_positive_prob, 0.0) << "False positive probability should be positive";
  CHECK_GT(elements, 0) << "Expected elements that would be inserted should be positive";
  int64_t bits = GetBitSize(elements, false_positive_prob);
  num_bytes_ = (bits+7)>>3;
  if (num_bytes_ > max_mem_usage*1024*1024) {
    num_bytes_ = max_mem_usage*1024*1024;
    LOG(WARNING) << "The memory requirement to fullfill the fasle positive probability exceeds the maximum memory limit."
                 << "The maximum memory is used and the false positive probability would be: "
                 << GetFalsePositiveProb(num_bytes_<<3, elements);
  }
  num_bits_ = num_bytes_<<3;
  num_seeds_ = GetHashFunctionSize(num_bits_, elements);
  LOG(INFO) << "number of bytes used: " << num_bytes_;
  LOG(INFO) << "number of hash functions used: " << num_seeds_;

  seeds_.reset(new uint32_t[num_seeds_]);
  srand(time(NULL));
  for (int32_t i = 0; i < num_seeds_; ++i) {
    seeds_[i] = rand();
  }

  data_.reset(new uint8_t[num_bytes_]);
  memset(data_.get(), 0, num_bytes_);
}

BloomFilter::BloomFilter(const std::string &path, HashFunction func): hash_func_(func) {
  CHECK_EQ(ReadFromFile(path), RETURN_OK) << "Failed to read data from file: " << path;
}

void BloomFilter::Insert(const void *key, int32_t len) {
  for (int32_t i = 0; i < num_seeds_; ++i) {
    uint64_t hash_val = hash_func_(key, len, seeds_[i]);
    uint64_t bit_index = hash_val % num_bits_;
    data_[bit_index >> 3] |= (1 << (bit_index & 7));
  }
}

bool BloomFilter::Exists(const void *key, int32_t len) {
  for (int32_t i = 0; i < num_seeds_; ++i) {
    uint64_t hash_val = hash_func_(key, len, seeds_[i]);
    uint64_t bit_index = hash_val % num_bits_;
    if (!(data_[bit_index >> 3] & (1 << (bit_index & 7)))) {
      return false;
    }
  }
  return true;
}
/*
 * Bloom filter file format
 *   1. time stamp <int64_t>
 *   2. number of seeds <int64_t>
 *   3. seed #1, seed #2, ... <uint32_t>
 *   4. number of bits <int64_t>
 *   5. number of bytes <int64_t>
 *   6. data <string, size: number of bytes>
 */
int32_t BloomFilter::WriteToFile(const std::string &path) {
  File file(path, OPEN_WRITE);
  if (!file.IsOpen()) return RETURN_ERR;
  int64_t time_stamp = time(NULL);
  file.WriteInt64(time_stamp);
  file.WriteInt64(num_seeds_);
  for (int32_t i = 0; i < num_seeds_; ++i) {
    file.WriteUInt32(seeds_[i]);
  }
  file.WriteInt64(num_bits_);
  file.WriteInt64(num_bytes_);
  file.WriteBytes(data_.get(), num_bytes_);
  return RETURN_OK;
}

int32_t BloomFilter::ReadFromFile(const std::string &path) {
  File file(path, OPEN_READ);
  if (!file.IsOpen()) return RETURN_ERR;
  int64_t time_stamp;
  file.ReadInt64(&time_stamp);
  LOG(INFO) << path << " saved at " << time_stamp;
  file.ReadInt64(&num_seeds_);
  seeds_.reset(new uint32_t[num_seeds_]);
  for (int32_t i = 0; i < num_seeds_; ++i) {
    file.ReadUInt32(&seeds_[i]);
  }
  file.ReadInt64(&num_bits_);
  file.ReadInt64(&num_bytes_);
  LOG(INFO) << "number of bytes used: " << num_bytes_;
  LOG(INFO) << "number of hash functions used: " << num_seeds_;
  data_.reset(new uint8_t[num_bytes_]);
  file.ReadBytes(data_.get(), num_bytes_);
  return RETURN_OK;
}

}

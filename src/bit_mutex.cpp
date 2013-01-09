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

#include <algorithm>
#include <glog/logging.h>
#include <pthread.h>
#include "bit_mutex.hpp"

namespace netlib {
BitMutex::BitMutex(uint64_t bits): bits_(bits) {
  uint64_t bytes = ((bits_ + 7) >> 3);
  bitmap_ = new uint8_t[bytes];
  std::fill_n(bitmap_, bytes, 0);
}

BitMutex::~BitMutex() {
  delete[] bitmap_;
}

void BitMutex::Lock(uint64_t id, uint64_t try_count) const {
  CHECK_LT(id, bits_) << "id out of range";

  uint64_t byte_index = (id >> 3);
  uint64_t bit_offset = (id & 7);
  volatile uint8_t *ptr = bitmap_ + byte_index;
  uint8_t mask = (1 << bit_offset);
  uint64_t cnt = 0;
  while (true) {
    uint8_t oldval = *ptr;
    uint8_t newval = (oldval | mask);
    if (oldval & mask) {
      // the bit is hold by someone else
      // sleep for sometime?
    } else if (__sync_bool_compare_and_swap(ptr, oldval, newval)) {
      break;
    }
    cnt++;
    if (cnt >= try_count) {
      cnt = 0;
      pthread_yield();
    }
  }
}

void BitMutex::Unlock(uint64_t id) const {
  CHECK_LT(id, bits_) << "id out of range";

  uint64_t byte_index = (id >> 3);
  uint64_t bit_offset = (id & 7);
  volatile uint8_t *ptr = bitmap_ + byte_index;
  uint8_t mask = (1 << bit_offset);
  while(true) {
    uint8_t oldval = *ptr;
    uint8_t newval = (oldval & (~mask));
    if (__sync_bool_compare_and_swap(ptr, oldval, newval))
      break;
  }
}
}

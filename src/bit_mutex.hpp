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

#ifndef _BIT_MUTEX_H_
#define _BIT_MUTEX_H_

#include "config.hpp"
namespace netlib {
class BitMutex {
 public:
  BitMutex(uint64_t bits);
  ~BitMutex();
  void Lock(uint64_t id, uint64_t try_count) const;
  void Unlock(uint64_t id) const;
 private:
  mutable uint8_t *bitmap_;
  uint64_t bits_;

  DISALLOW_COPY_AND_ASSIGN(BitMutex);
};

class ScopedBitMutexLock {
 public:
  ScopedBitMutexLock(const BitMutex &mu,
                     uint64_t id,
                     uint64_t try_count = 100):mutex_(&mu), id_(id) {
    mutex_->Lock(id_, try_count);
  }
  ~ScopedBitMutexLock() {
    mutex_->Unlock(id_);
  }
 private:
  const BitMutex *mutex_;
  uint64_t id_;
  DISALLOW_COPY_AND_ASSIGN(ScopedBitMutexLock);
};
}

#endif /* _BIT_MUTEX_H_ */

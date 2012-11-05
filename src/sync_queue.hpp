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
#ifndef _SYNC_QUEUE_H_
#define _SYNC_QUEUE_H_
#include <queue>
#include <boost/scoped_ptr.hpp>
#include "mutex.hpp"

namespace netlib {
template <typename T>
class SyncQueue {
 public:
  SyncQueue(uint32_t limites);
  void Pop(T *val_ptr);
  void Push(const T &val);
  bool TryPop(T *val_ptr);

  uint32_t Size() const;
  bool Empty() const;
 private:
  std::queue<T> queue_;
  uint32_t limites_;
  boost::scoped_ptr<Mutex> mu_;
  boost::scoped_ptr<Cond> cond1_;
  boost::scoped_ptr<Cond> cond2_;
};

template <typename T>
SyncQueue<T>::SyncQueue(uint32_t limites) {
  limites_ = limites;
  mu_.reset(new Mutex);
  cond1_.reset(new Cond(*mu_));
  cond2_.reset(new Cond(*mu_));
}

template <typename T>
void SyncQueue<T>::Pop(T *val_ptr) {
  ScopedMutexLock lock(*mu_);
  while (queue_.empty()) {
    cond1_->Wait();
  }
  *val_ptr = queue_.front();
  queue_.pop();
  cond2_->Notify();
}

template <typename T>
void SyncQueue<T>::Push(const T &val) {
  ScopedMutexLock lock(*mu_);
  while(queue_.size() >= limites_) {
    cond2_->Wait();
  }
  queue_.push(val);
  cond1_->Notify();
}

template <typename T>
bool SyncQueue<T>::TryPop(T *val_ptr) {
  ScopedMutexLock lock(*mu_);
  if (queue_.empty()) {
    return false;
  }
  *val_ptr = queue_.front();
  queue_.pop();
  cond2_->Notify();
  return true;
}

template <typename T>
uint32_t SyncQueue<T>::Size() const {
  ScopedMutexLock lock(*mu_);
  return queue_.size();
}

template <typename T>
bool SyncQueue<T>::Empty() const {
  ScopedMutexLock lock(*mu_);
  return queue_.empty();
}

}

#endif /* _SYNC_QUEUE_H_ */

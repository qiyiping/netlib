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
#ifndef _MUTEX_H_
#define _MUTEX_H_
#include "config.hpp"
#include <pthread.h>
#include "time.hpp"
#include <glog/logging.h>

namespace netlib {
class Mutex {
 public:
  Mutex() {
    // use `pthread_mutex_init instead' in order to check errors
    // mutex_ = PTHREAD_MUTEX_INITIALIZER;
    CHECK_EQ(pthread_mutex_init(&mutex_, NULL), 0);
  }
  void Lock() const {
    pthread_mutex_lock(&mutex_);
  }

  void Unlock() const {
    pthread_mutex_unlock(&mutex_);
  }

  bool TryLock() const {
    return (pthread_mutex_trylock(&mutex_) == 0);
  }

  bool TimedLock(int64_t ms) const {
    int64_t now = GetMilliSeconds();
    int64_t till = now+ms;
    struct timespec abs_timeout;
    MilliSecondsToTimespec(till, &abs_timeout);
    return (pthread_mutex_timedlock(&mutex_, &abs_timeout) == 0);
  }

  ~Mutex() { pthread_mutex_destroy(&mutex_); }
 private:
  friend class Cond;
  mutable pthread_mutex_t mutex_;
  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

class ScopedMutexLock {
 public:
  ScopedMutexLock(const Mutex &m):mutex_(&m) {
    mutex_->Lock();
  }
  ~ScopedMutexLock() {
    mutex_->Unlock();
  }
 private:
  const Mutex *mutex_;
  DISALLOW_COPY_AND_ASSIGN(ScopedMutexLock);
};

class ReadWriteMutex {
 public:
  ReadWriteMutex() {
    CHECK_EQ(pthread_rwlock_init(&rwlock_, NULL), 0);
  }

  void ReadLock() const {
    pthread_rwlock_rdlock(&rwlock_);
  }

  void WriteLock() const {
    pthread_rwlock_wrlock(&rwlock_);
  }

  bool TryReadLock() const {
    return (pthread_rwlock_tryrdlock(&rwlock_) == 0);
  }

  bool TryWriteLock() const {
    return (pthread_rwlock_trywrlock(&rwlock_) == 0);
  }

  bool TimedReadLock(int64_t ms) const {
    int64_t now = GetMilliSeconds();
    int64_t till = now+ms;
    struct timespec abs_timeout;
    MilliSecondsToTimespec(till, &abs_timeout);
    return (pthread_rwlock_timedrdlock(&rwlock_, &abs_timeout) == 0);
  }

  bool TimedWriteLock(int64_t ms) const {
    int64_t now = GetMilliSeconds();
    int64_t till = now+ms;
    struct timespec abs_timeout;
    MilliSecondsToTimespec(till, &abs_timeout);
    return (pthread_rwlock_timedwrlock(&rwlock_, &abs_timeout) == 0);
  }

  void Unlock() const {
    pthread_rwlock_unlock(&rwlock_);
  }

  ~ReadWriteMutex() {
    pthread_rwlock_destroy(&rwlock_);
  }
 private:
  mutable pthread_rwlock_t rwlock_;
  DISALLOW_COPY_AND_ASSIGN(ReadWriteMutex);
};

class ScopedReadLock {
 public:
  ScopedReadLock(const ReadWriteMutex &m):rwmutex_(&m) {
    rwmutex_->ReadLock();
  }
  ~ScopedReadLock() {
    rwmutex_->Unlock();
  }
 private:
  const ReadWriteMutex *rwmutex_;
  DISALLOW_COPY_AND_ASSIGN(ScopedReadLock);
};

class ScopedWriteLock {
 public:
  ScopedWriteLock(const ReadWriteMutex &m):rwmutex_(&m) {
    rwmutex_->WriteLock();
  }
  ~ScopedWriteLock() {
    rwmutex_->Unlock();
  }
 private:
  const ReadWriteMutex *rwmutex_;
  DISALLOW_COPY_AND_ASSIGN(ScopedWriteLock);
};

class Cond {
 public:
  Cond(Mutex &m):mutex_(&m) {
    CHECK_EQ(pthread_cond_init(&cond_, NULL), 0);
  }

  void Wait() {
    pthread_cond_wait(&cond_, &(mutex_->mutex_));
  }

  bool TimedWait(int64_t ms) {
    int64_t now = GetMilliSeconds();
    int64_t till = now+ms;
    struct timespec abs_timeout;
    MilliSecondsToTimespec(till, &abs_timeout);
    return (pthread_cond_timedwait(&cond_, &(mutex_->mutex_), &abs_timeout) == 0);
  }

  void Notify() {
    pthread_cond_signal(&cond_);
  }

  void NotifyAll() {
    pthread_cond_broadcast(&cond_);
  }

  ~Cond() {
    pthread_cond_destroy(&cond_);
  }
 private:
  Mutex *mutex_;
  pthread_cond_t cond_;
  DISALLOW_COPY_AND_ASSIGN(Cond);
};

}
#endif /* _MUTEX_H_ */

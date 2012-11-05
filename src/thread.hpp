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
#ifndef _THREAD_H_
#define _THREAD_H_

#include "config.hpp"
#include <pthread.h>
#include <glog/logging.h>

namespace netlib{
class Thread {
 public:
  enum State {
    UNINITIALIZED,
    RUNNING,
    STOPPED,
  };

  Thread(bool detached):detached_(detached), state_(UNINITIALIZED) {}

  void Start();
  void Join();

  pthread_t GetId() { return thread_; }

  virtual ~Thread() {
    if (!detached_ && state_ == RUNNING) {
      Join();
    }
  }

  static void *ThreadMain(void *arg);

 protected:
  virtual void Run() = 0;
 private:
  pthread_t thread_;
  bool detached_;
  State state_;
};

inline
void Thread::Start() {
  if (state_ != UNINITIALIZED) {
    LOG(ERROR) << "thread is initialized";
    return;
  }
  pthread_attr_t attr;
  CHECK_EQ(pthread_attr_init(&attr), 0);
  if (detached_) {
    CHECK_EQ(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED), 0);
  } else {
    CHECK_EQ(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE), 0);
  }
  CHECK_EQ(pthread_create(&thread_, &attr, Thread::ThreadMain, this), 0);
  state_ = RUNNING;
  pthread_attr_destroy(&attr);
}

inline
void Thread::Join() {
  if (state_ != RUNNING) {
    LOG(ERROR) << "thread is not running";
    return;
  }
  if (!detached_) {
    void *value;
    int32_t ret = pthread_join(thread_, &value);
    if ( ret != 0) {
      LOG(ERROR) << "pthread_join failed";
    }
  }
  state_ = STOPPED;
}

inline
void *Thread::ThreadMain(void *arg) {
  Thread *thread = reinterpret_cast<Thread *>(arg);
  thread->Run();
  return NULL;
}

}
#endif /* _THREAD_H_ */

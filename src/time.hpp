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
#ifndef _TIME_H_
#define _TIME_H_
#include <sys/time.h>
#include <time.h>
#include <glog/logging.h>
#include <string>
#include "config.hpp"


namespace netlib {
inline int64_t GetMilliSeconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec*1000+tv.tv_usec/1000);
}

inline int64_t GetMicroSeconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec*1000000+tv.tv_usec);
}

inline void MilliSecondsToTimeval(int64_t ms, struct timeval *tv) {
  tv->tv_sec = ms/1000;
  tv->tv_usec = (ms%1000)*1000;
}

inline void MicroSecondsToTimeval(int64_t us, struct timeval *tv) {
  tv->tv_sec = us/1000000;
  tv->tv_usec = (us%1000000);
}

inline void MilliSecondsToTimespec(int64_t ms, struct timespec *ts) {
  ts->tv_sec = ms/1000;
  ts->tv_nsec = (ms%1000)*1000000;
}

inline void MicroSeccondsToTimespec(int64_t us, struct timespec *ts) {
  ts->tv_sec = us/1000000;
  ts->tv_nsec = (us%1000000)*1000;
}

class Timer {
 public:
  Timer(const std::string &hint): hint_(hint) { time_ = GetMilliSeconds(); }
  Timer() { time_ = GetMilliSeconds(); }
  int64_t GetElapsedTime() { return GetMilliSeconds() - time_; }
  ~Timer() { LOG(INFO) << hint_ << GetElapsedTime(); }
 private:
  int64_t time_;
  const std::string hint_;
};

}

#endif /* _TIME_H_ */

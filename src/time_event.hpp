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
#ifndef _TIME_EVENT_H_
#define _TIME_EVENT_H_

#include "config.hpp"
#include <tr1/functional>
#include <vector>
#include <list>
#include "time.hpp"

namespace netlib {

class EventLoop;
typedef std::tr1::function<void (EventLoop *, int32_t)> TimeCallback;

const int32_t kTimeEventSetSize = 1024;

struct TimeEvent {
  int64_t scheduled_time;
  int64_t period;
  TimeCallback func;
  bool occupied;

  TimeEvent(): scheduled_time(-1), period(-1), func(NULL), occupied(false) {}
  void Clear() {
    scheduled_time = -1;
    period = -1;
    func = NULL;
    occupied = false;
  }
  bool Occupied() { return occupied; }
};

struct FiredTimeEvent {
  int32_t id;
  int64_t scheduled_time;
  bool operator < (const FiredTimeEvent &rhs) const {
    return scheduled_time < rhs.scheduled_time;
  }
};

}
#endif /* _TIME_EVENT_H_ */

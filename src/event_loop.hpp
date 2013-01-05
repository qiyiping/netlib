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

#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_

#include "config.hpp"
#include "socket_event.hpp"
#include "time_event.hpp"
#include <boost/shared_ptr.hpp>

namespace netlib {
class EventLoop {
 public:
  EventLoop(boost::shared_ptr<SocketEventHandler> handler):socket_event_handler_(handler),
                                                           stop_(false) {}
  void Main();
  void SetStop() { stop_ = true; }
  void UnsetStop() { stop_ = false; }

  int32_t AddSocketEvent(int32_t fd, uint32_t mask, const SocketCallback &rfunc, const SocketCallback &wfunc);
  int32_t ModifySocketEvent(int32_t fd, uint32_t mask, const SocketCallback &rfunc, const SocketCallback &wfunc);
  int32_t ModifySocketReadEvent(int32_t fd, const SocketCallback &rfunc);
  int32_t ModifySocketWriteEvent(int32_t fd, const SocketCallback &wfunc);
  int32_t DeleteSocketEvent(int32_t fd);

  int32_t AddTimeEvent(int64_t sch_time, int64_t period, const TimeCallback &func);
  int32_t ModifyTimeEvent(int32_t id, int64_t sch_time, int64_t period, const TimeCallback &func);
  int32_t DeleteTimeEvent(int32_t id);
  virtual ~EventLoop() {}
 protected:
  SocketEvent socket_events_[kSocketEventSetSize];
  FiredSocketEvent fired_socket_events_[kSocketEventSetSize];
  boost::shared_ptr<SocketEventHandler> socket_event_handler_;

  TimeEvent time_events_[kTimeEventSetSize];
  FiredTimeEvent fired_time_events_[kTimeEventSetSize];

  bool stop_;

  DISALLOW_COPY_AND_ASSIGN(EventLoop);
};

}

#endif /* _EVENT_LOOP_H_ */

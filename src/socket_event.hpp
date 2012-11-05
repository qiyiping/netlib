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
#ifndef _SOCKET_EVENT_H_
#define _SOCKET_EVENT_H_

#include "config.hpp"
#include <tr1/functional>
namespace netlib {

class EventLoop;
typedef std::tr1::function<void (EventLoop *, int)> SocketCallback;

const int32_t kSocketEventSetSize = 10*1024;

enum SocketEventType {
  EVENT_NONE = 0,
  EVENT_READ = 1,
  EVENT_WRITE = 2,
};

struct SocketEvent {
  uint32_t mask;
  SocketCallback read_func;
  SocketCallback write_func;
  SocketEvent():mask(EVENT_NONE), read_func(NULL), write_func(NULL) {}
};

struct FiredSocketEvent {
  int32_t fd;
  uint32_t mask;
};

class SocketEventHandler {
 public:
  /**
   * Register event
   *
   * @param fd file discriptor to be polled
   * @param mask event type to be monitored
   *
   * @return RETURN_OK if succeed, RETURN_ERR otherwise
   */
  virtual int32_t RegisterEvent(int32_t fd, uint32_t mask) = 0;

  /**
   * Unregister event
   *
   * @param fd file discriptor
   * @return RETURN_OK if succeed, RETURN_ERR otherwise
   */
  virtual int32_t UnregisterEvent(int32_t fd) = 0;

  /**
   * Modify a registered event
   *
   * @param fd file discriptor
   * @param mask event type
   *
   * @return RETURN_OK if succeed, RETURN_ERR otherwise
   */
  virtual int32_t ModifyEvent(int32_t fd, uint32_t mask) = 0;

  /**
   * Poll event
   *
   * @param fired_ev fired event pool
   * @param msecs milliseconds to wait
   *
   * @return number of fired events
   */
  virtual int32_t PollEvent(FiredSocketEvent *fired_ev, int32_t msecs) = 0;

  virtual ~SocketEventHandler() {}
};

}

#endif /* _SOCKET_EVENT_H_ */

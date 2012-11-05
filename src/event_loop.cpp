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

#include "event_loop.hpp"
#include <glog/logging.h>
#include <algorithm>
namespace netlib {

int32_t EventLoop::AddSocketEvent(int32_t fd, uint32_t mask, const SocketCallback &rfunc, const SocketCallback &wfunc) {
  if (mask == EVENT_NONE) {
    LOG(WARNING) << "mask is EVENT_NONE";
    return RETURN_ERR;
  }
  if (socket_events_[fd].mask != EVENT_NONE) {
    LOG(WARNING) << "fd is already added: " << fd;
    return RETURN_ERR;
  }
  int32_t ret = socket_event_handler_->RegisterEvent(fd, mask);
  if (ret == RETURN_OK) {
    socket_events_[fd].mask = mask;
    socket_events_[fd].read_func = rfunc;
    socket_events_[fd].write_func = wfunc;
  }
  return ret;
}

int32_t EventLoop::ModifySocketEvent(int32_t fd, uint32_t mask, const SocketCallback &rfunc, const SocketCallback &wfunc) {
  if (mask == EVENT_NONE) {
    LOG(WARNING) << "mask is EVENT_NONE";
    return RETURN_ERR;
  }

  if (socket_events_[fd].mask == EVENT_NONE) {
    LOG(WARNING) << "fd isn't added yet: " << fd;
    return RETURN_ERR;
  }

  if (socket_events_[fd].mask == mask) {
    socket_events_[fd].read_func = rfunc;
    socket_events_[fd].write_func = wfunc;
    return RETURN_OK;
  }

  int32_t ret = socket_event_handler_->ModifyEvent(fd, mask);
  if (ret == RETURN_OK) {
    socket_events_[fd].mask = mask;
    socket_events_[fd].read_func = rfunc;
    socket_events_[fd].write_func = wfunc;
  }
  return ret;
}

int32_t EventLoop::ModifySocketReadEvent(int32_t fd, const SocketCallback &rfunc) {
  uint32_t new_mask = socket_events_[fd].mask;
  if (rfunc) {
    new_mask |= EVENT_READ;
  } else {
    new_mask &= (~EVENT_READ);
  }
  return ModifySocketEvent(fd,
                           new_mask,
                           rfunc,
                           socket_events_[fd].write_func);
}

int32_t EventLoop::ModifySocketWriteEvent(int32_t fd, const SocketCallback &wfunc) {
  uint32_t new_mask = socket_events_[fd].mask;
  if (wfunc) {
    new_mask |= EVENT_WRITE;
  } else {
    new_mask &= (~EVENT_WRITE);
  }
  return ModifySocketEvent(fd,
                           new_mask,
                           socket_events_[fd].read_func,
                           wfunc);
}

int32_t EventLoop::DeleteSocketEvent(int32_t fd) {
  if (socket_events_[fd].mask == EVENT_NONE) {
    LOG(WARNING) << "fd isn't added yet: " << fd;
    return RETURN_ERR;
  }
  // int32_t ret = socket_event_handler_->UnregisterEvent(fd);
  // if (ret == RETURN_OK) {
  //   socket_events_[fd].mask = EVENT_NONE;
  //   socket_events_[fd].read_func = NULL;
  //   socket_events_[fd].write_func = NULL;
  // }
  // return ret;
  socket_event_handler_->UnregisterEvent(fd);
  socket_events_[fd].mask = EVENT_NONE;
  socket_events_[fd].read_func = NULL;
  socket_events_[fd].write_func = NULL;
  return RETURN_OK;
}

int32_t EventLoop::AddTimeEvent(int64_t sch_time, int64_t period, const TimeCallback &func) {
  int id = 0;
  while (id < kTimeEventSetSize && time_events_[id].Occupied()) {
    ++id;
  }
  if (id >= kTimeEventSetSize) {
    LOG(WARNING) << "time event set is full";
    return -1;
  }
  time_events_[id].scheduled_time = sch_time;
  time_events_[id].period = period;
  time_events_[id].func = func;
  time_events_[id].occupied = true;
  return id;
}

int32_t EventLoop::ModifyTimeEvent(int32_t id, int64_t sch_time, int64_t period, const TimeCallback &func) {
  if (id < kTimeEventSetSize && time_events_[id].Occupied()) {
    time_events_[id].scheduled_time = sch_time;
    time_events_[id].period = period;
    time_events_[id].func = func;
    return RETURN_OK;
  } else {
    LOG(WARNING) << "time event with id " << id << "is not registered";
    return RETURN_ERR;
  }
}

int32_t EventLoop::DeleteTimeEvent(int32_t id) {
  if (id < kTimeEventSetSize) {
    time_events_[id].Clear();
  }
  return RETURN_OK;
}

void EventLoop::Main() {
  while (!stop_) {
    // time event
    int32_t nevs = 0;
    int64_t now = GetMilliSeconds();
    int64_t next_sched_time = now + 10;

    for (int32_t i = 0; i < kTimeEventSetSize; ++i) {
      if (time_events_[i].Occupied()){
        if (time_events_[i].scheduled_time < now) {
          fired_time_events_[nevs].id = i;
          fired_time_events_[nevs].scheduled_time = time_events_[i].scheduled_time;
          nevs++;
        } else if (next_sched_time > time_events_[i].scheduled_time) {
          next_sched_time = time_events_[i].scheduled_time;
        }
      }
    }

    std::sort(fired_time_events_, fired_time_events_+nevs);
    for (int32_t i = 0; i < nevs; ++i) {
      int32_t id = fired_time_events_[i].id;
      time_events_[id].func(this, id);

      if (time_events_[id].period > 0) {
        time_events_[id].scheduled_time = now + time_events_[id].period;
        if (next_sched_time > time_events_[id].scheduled_time) {
          next_sched_time = time_events_[id].scheduled_time;
        }
      } else {
        DeleteTimeEvent(id);
      }

    }

    now = GetMilliSeconds();
    int32_t wait_time = next_sched_time > now? next_sched_time - now:0;
    // socket event
    nevs = socket_event_handler_->PollEvent(fired_socket_events_, wait_time);
    for (int32_t i = 0; i < nevs; ++i) {
      int32_t fd = fired_socket_events_[i].fd;
      uint32_t mask = fired_socket_events_[i].mask;
      bool dealed = false;

      // deal with request first
      if ((socket_events_[fd].mask & mask & EVENT_READ) &&
          socket_events_[fd].read_func) {
        socket_events_[fd].read_func(this, fd);
        dealed = true;
      }

      if (!dealed &&
          (socket_events_[fd].mask & mask & EVENT_WRITE) &&
          socket_events_[fd].write_func) {
        socket_events_[fd].write_func(this, fd);
      }

    }

  } // while loop
}

}

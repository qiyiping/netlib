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

#include "epoll_socket_handler.hpp"
#include <glog/logging.h>

namespace netlib {
EpollSocketEventHandler::EpollSocketEventHandler() {
  epoll_fd_ = epoll_create(1024);
  CHECK_NE(epoll_fd_, -1) << "epoll_create: failed to create a epoll instance";
}

EpollSocketEventHandler::~EpollSocketEventHandler() {
  close(epoll_fd_);
}

int32_t EpollSocketEventHandler::RegisterEvent(int32_t fd, uint32_t mask) {
  struct epoll_event ee;
  ee.data.u64 = 0;
  ee.data.fd = fd;
  ee.events = 0;
  if (mask & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (mask & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ee) == -1) {
    LOG(ERROR) << "epoll_ctl: failed to add fd: " << fd;
    return RETURN_ERR;
  } else {
    return RETURN_OK;
  }
}

int32_t EpollSocketEventHandler::UnregisterEvent(int32_t fd) {
  struct epoll_event ee;
  ee.data.u64 = 0;
  ee.data.fd = fd;
  ee.events = 0;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, &ee) == -1) {
    LOG(ERROR) << "epoll_ctl: failed to remove fd: " << fd;
    return RETURN_ERR;
  } else {
    return RETURN_OK;
  }
}

int32_t EpollSocketEventHandler::ModifyEvent(int32_t fd, uint32_t mask) {
  struct epoll_event ee;
  ee.data.u64 = 0;
  ee.data.fd = fd;
  ee.events = 0;
  if (mask & EVENT_READ) {
    ee.events |= EPOLLIN;
  }
  if (mask & EVENT_WRITE) {
    ee.events |= EPOLLOUT;
  }
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ee) == -1) {
    LOG(ERROR) << "epoll_ctl: failed to modify fd: " << fd;
    return RETURN_ERR;
  } else {
    return RETURN_OK;
  }
}

int32_t EpollSocketEventHandler::PollEvent(FiredSocketEvent *fired_ev, int32_t msecs) {
  int32_t nfds = 0;
  if (msecs < 0) {
    msecs = -1;
  }
  nfds = epoll_wait(epoll_fd_, events_, kSocketEventSetSize, msecs);
  if (nfds == -1) {
    LOG(ERROR) << "epoll_wait: failed";
    return -1;
  }
  for (int i = 0; i < nfds; ++i) {
    fired_ev[i].fd = events_[i].data.fd;
    uint32_t mask = 0;
    if (events_[i].events & EPOLLIN) {
      mask |= EVENT_READ;
    }
    if (events_[i].events & EPOLLOUT) {
      mask |= EVENT_WRITE;
    }
    fired_ev[i].mask = mask;
  }
  return nfds;
}

}

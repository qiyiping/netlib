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
#include "socket_io.hpp"
#include <errno.h>
#include <sys/select.h>
#include "time.hpp"

namespace netlib {
int32_t SocketIO::ReadBytes(void *ptr, uint32_t size) {
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(socket_fd_, &rfds);
  struct timeval tv;
  struct timeval *timeout = NULL;
  if (recv_timeout_ >= 0) {
    MilliSecondsToTimeval(recv_timeout_, &tv);
    timeout = &tv;
  }
  int32_t nfds = 0;

  while (true) {
    nfds = select(socket_fd_+1, &rfds, NULL, NULL, timeout);
    if (nfds < 0) {
      int32_t errno_copy = errno;
      if (errno_copy == EINTR || errno_copy == EAGAIN || errno_copy == EWOULDBLOCK) {
        continue;
      }
    }
    break;
  }

  if (nfds == 0) // timeout expires
    return -1;

  if (!FD_ISSET(socket_fd_, &rfds))
    return -1;

  int32_t recv_ret = recv(socket_fd_, ptr, size, 0);
  return recv_ret;
}

int32_t SocketIO::WriteBytes(const void *ptr, uint32_t size) {
  fd_set wfds;
  FD_ZERO(&wfds);
  FD_SET(socket_fd_, &wfds);
  struct timeval tv;
  struct timeval *timeout = NULL;
  if (send_timeout_ >= 0) {
    MilliSecondsToTimeval(send_timeout_, &tv);
    timeout = &tv;
  }
  int32_t nfds = 0;

  while (true) {
    nfds = select(socket_fd_+1, NULL, &wfds, NULL, timeout);
    if (nfds < 0) {
      int32_t errno_copy = errno;
      if (errno_copy == EINTR || errno_copy == EAGAIN || errno_copy == EWOULDBLOCK) {
        continue;
      }
    }
    break;
  }

  if (nfds == 0) // timeout expires
    return -1;

  if (!FD_ISSET(socket_fd_, &wfds))
    return -1;

  int32_t send_ret = send(socket_fd_, ptr, size, 0);
  return send_ret;
}

int32_t SocketIO::ReadString(std::string *str) {
  char buf[1024*1024];
  str->clear();
  int32_t ret = ReadBytes(buf, 1024*1024);
  if (ret > 0) {
    str->assign(buf, ret);
  }
  return ret;
}

int32_t SocketIO::WriteString(const std::string &str) {
  return WriteBytes(str.c_str(), str.length());
}

bool SocketIO::Peek() {
  char buf[1];
  int32_t ret = recv(socket_fd_, buf, 1, MSG_PEEK);
  if (ret == -1) {
    int32_t errno_copy = errno;
    if (errno_copy == ECONNRESET) {
      close(socket_fd_);
      return false;
    }
  }
  return ret != 0;
}

}

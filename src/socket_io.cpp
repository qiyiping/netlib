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
#include "time.hpp"

namespace netlib {
int32_t SocketIO::ReadBytes(void *ptr, uint32_t size) {
  int32_t retry_time = 0;
  int64_t end_time = GetMilliSeconds() + recv_timeout_;
  int32_t ret = 0;
  while ((retry_time++) < max_retry_time_) {
    ret = recv(socket_fd_, ptr, size, recv_flag_);
    if (ret >= 0){
      break;
    } else if (ret == -1) {
      int32_t errno_copy = errno;
      if (errno_copy == EAGAIN) {
        usleep(kSleepUsecs);
      } else {
        break;
      }
    }
    if (GetMilliSeconds() > end_time) { // timeout
      break;
    }
  }
  return ret;
}

int32_t SocketIO::WriteBytes(const void *ptr, uint32_t size) {
  int32_t retry_time = 0;
  int64_t end_time = GetMilliSeconds() + recv_timeout_;
  int32_t ret = 0;
  while ((retry_time++) < max_retry_time_) {
    ret = send(socket_fd_, ptr, size, send_flag_);
    if (ret >= 0){
      break;
    } else if (ret == -1) {
      int32_t errno_copy = errno;
      if (errno_copy == EAGAIN){
        usleep(kSleepUsecs);
      } else {
        break;
      }
    }
    if (GetMilliSeconds() > end_time) { // timeout
      break;
    }
  }
  return ret;
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
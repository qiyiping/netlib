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
#include "net.hpp"
#include <glog/logging.h>

namespace netlib {
int32_t SetSocketNonblocking(int32_t fd) {
  int32_t flag = fcntl(fd, F_GETFL, 0);
  if (flag == -1) {
    return RETURN_ERR;
  }
  flag |= O_NONBLOCK;
  int32_t ret = fcntl(fd, F_SETFL, flag);
  if (ret == -1) {
    return RETURN_ERR;
  }
  return RETURN_OK;
}

int32_t Connect(const std::string &addr, const std::string &port, bool nonblocking) {
  struct addrinfo hints, *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  int32_t status = getaddrinfo(addr.c_str(), port.c_str(), &hints, &result);
  if (status != 0) {
    LOG(ERROR) << gai_strerror(status);
    freeaddrinfo(result);
    return -1;
  }

  int32_t fd = -1;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (fd == -1) {
      LOG(WARNING) << "failed to create a socket.";
      continue;
    }

    if (connect(fd, rp->ai_addr, rp->ai_addrlen) != 0) {
      LOG(WARNING) << "failed to connect.";
      close(fd);
      continue;
    }

    if (!nonblocking || SetSocketNonblocking(fd) == RETURN_OK)
      break;

    LOG(WARNING) << "failed to set socket fd nonblocking.";
    close(fd);
  }

  if (!rp) {
    LOG(ERROR) << "could not connect";
    fd = -1;
  }

  freeaddrinfo(result);
  return fd;
}
}

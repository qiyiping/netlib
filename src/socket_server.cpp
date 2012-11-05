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
#include "socket_server.hpp"
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>

namespace netlib {
SocketServer::SocketServer(const std::string &addr, const std::string &port, bool nonblocking) {
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  // hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  const char *addr_ptr = NULL;
  if (!addr.empty())
    addr_ptr = addr.c_str();
  const char *port_ptr = NULL;
  if (!port.empty())
    port_ptr = port.c_str();
  int32_t status = getaddrinfo(addr_ptr, port_ptr, &hints, &result);
  CHECK_EQ(status, 0) << gai_strerror(status);

  for (rp = result; rp != NULL; rp = rp->ai_next) {
    listener_fd_ = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if (listener_fd_ == -1) {
      LOG(WARNING) << "failed to create a socket.";
      continue;
    }

    if (bind(listener_fd_, rp->ai_addr, rp->ai_addrlen) != 0) {
      LOG(WARNING) << "failed to bind.";
      close(listener_fd_);
      continue;
    }

    if (!nonblocking || SetSocketNonblocking(listener_fd_) == RETURN_OK)
      break;

    LOG(WARNING) << "failed to set socket fd nonblocking.";
    close(listener_fd_);
  }

  CHECK(rp) << "could not bind(2)";

  char ipstr[INET6_ADDRSTRLEN];
  void *in_addr = NULL;
  int32_t in_port = 0;
  if (rp->ai_family == AF_INET) {
    ip_version_ = "IPv4";
    struct sockaddr_in *v4 = reinterpret_cast<sockaddr_in *>(rp->ai_addr);
    in_addr = &(v4->sin_addr);
    in_port = ntohs(v4->sin_port);
  } else {
    ip_version_ = "IPv6";
    struct sockaddr_in6 *v6 = reinterpret_cast<sockaddr_in6 *>(rp->ai_addr);
    in_addr = &(v6->sin6_addr);
    in_port = ntohs(v6->sin6_port);
  }

  inet_ntop(rp->ai_family, in_addr, ipstr, sizeof(ipstr));
  address_.assign(ipstr);

  port_ = boost::lexical_cast<std::string>(in_port);

  LOG(INFO) << "server address: " << address_;
  LOG(INFO) << "server port: " << port_;
  LOG(INFO) << "address family: " << ip_version_;

  freeaddrinfo(result);
}

void SocketServer::Listen() {
  int32_t ret = listen(listener_fd_, SOMAXCONN);
  CHECK_EQ(ret, 0) << "could not listen(2)";
}

SocketServer::~SocketServer() {
  close(listener_fd_);
}

}

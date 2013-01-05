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
#include <boost/lexical_cast.hpp>
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

void ParseAddressInfo(const struct addrinfo *rp,
                      std::string *version,
                      std::string *address,
                      std::string *port) {
  char ipstr[INET6_ADDRSTRLEN];
  void *in_addr = NULL;
  int32_t in_port = 0;
  if (rp->ai_family == AF_INET) {
    *version = "IPv4";
    struct sockaddr_in *v4 = reinterpret_cast<sockaddr_in *>(rp->ai_addr);
    in_addr = &(v4->sin_addr);
    in_port = ntohs(v4->sin_port);
  } else {
    *version = "IPv6";
    struct sockaddr_in6 *v6 = reinterpret_cast<sockaddr_in6 *>(rp->ai_addr);
    in_addr = &(v6->sin6_addr);
    in_port = ntohs(v6->sin6_port);
  }

  inet_ntop(rp->ai_family, in_addr, ipstr, sizeof(ipstr));
  address->assign(ipstr);

  *port = boost::lexical_cast<std::string>(in_port);
}

int32_t CreateClientSocket(const std::string &addr, const std::string &port) {
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

    if (connect(fd, rp->ai_addr, rp->ai_addrlen) == 0)
      break;

    LOG(WARNING) << "failed to connect.";
    close(fd);
  }

  if (!rp) {
    LOG(ERROR) << "could not create client socket";
    fd = -1;
  } else {
    std::string version, address, port;
    ParseAddressInfo(rp, &version, &address, &port);
    LOG(INFO) << "protocol version: " << version;
    LOG(INFO) << "address: " << address;
    LOG(INFO) << "port: " << port;
  }

  freeaddrinfo(result);
  return fd;
}

int32_t CreateServerSocket(const std::string &addr, const std::string &port) {
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  const char *addr_ptr = NULL;
  if (!addr.empty())
    addr_ptr = addr.c_str();
  const char *port_ptr = NULL;
  if (!port.empty())
    port_ptr = port.c_str();
  int32_t status = getaddrinfo(addr_ptr, port_ptr, &hints, &result);
  if (status != 0) {
    LOG(ERROR) << gai_strerror(status);
    freeaddrinfo(result);
    return -1;
  }

  int fd = -1;
  for (rp = result; rp != NULL; rp = rp->ai_next) {
    fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

    if (fd == -1) {
      LOG(WARNING) << "failed to create a socket.";
      continue;
    }

    if (bind(fd, rp->ai_addr, rp->ai_addrlen) == 0)
      break;

    LOG(WARNING) << "failed to bind.";
    close(fd);
  }

  if (!rp) {
    LOG(ERROR) << "could not create server socket";
    fd = -1;
  } else {
    std::string version, address, port;
    ParseAddressInfo(rp, &version, &address, &port);
    LOG(INFO) << "protocol version: " << version;
    LOG(INFO) << "address: " << address;
    LOG(INFO) << "port: " << port;
  }

  freeaddrinfo(result);
  return fd;
}
}

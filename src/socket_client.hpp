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
#ifndef _SOCKET_CLIENT_H_
#define _SOCKET_CLIENT_H_
#include "config.hpp"
#include <string>

namespace netlib {
class SocketClient {
 public:
  SocketClient(const std::string &address,
               const std::string &port,
               bool nonblocking = false);
  int32_t GetSocket() const { return fd_; }
  bool Open(bool force_open = false);
  void Close();
  bool IsConnected() const { return fd_ >= 0; }
  std::string GetPeerAddress() const { return peer_address_; }
  std::string GetPeerPort() const { return peer_port_; }
  bool IsBlocking() const { return !nonblocking_; }
  virtual ~SocketClient() { Close(); }

 private:
  int32_t fd_;
  std::string peer_address_;
  std::string peer_port_;
  bool nonblocking_;

  DISALLOW_COPY_AND_ASSIGN(SocketClient);
};
}

#endif /* _SOCKET_CLIENT_H_ */

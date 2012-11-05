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
#ifndef _THREAD_POOL_SOCKET_SERVER_H_
#define _THREAD_POOL_SOCKET_SERVER_H_
#include <boost/shared_ptr.hpp>
#include "socket_server.hpp"
#include "thread_pool.hpp"
#include "request_handler.hpp"

namespace netlib {

class TPSocketServer:public SocketServer {
 public:
  TPSocketServer(const std::string &addr,
                 const std::string &port,
                 boost::shared_ptr<RequestHandler> handler,
                 boost::shared_ptr<ThreadPool> thread_pool):
      SocketServer(addr, port, false),
      request_handler_(handler),
      thread_pool_(thread_pool) {}

  void Serve();
 protected:
  void Handle(int32_t fd);

  boost::shared_ptr<RequestHandler> request_handler_;
  boost::shared_ptr<ThreadPool> thread_pool_;
 private:
  DISALLOW_COPY_AND_ASSIGN(TPSocketServer);
};
}

#endif /* _THREAD_POOL_SOCKET_SERVER_H_ */

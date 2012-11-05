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
#ifndef _REQUEST_HANDLER_H_
#define _REQUEST_HANDLER_H_
#include "config.hpp"
#include <string>
#include <boost/shared_ptr.hpp>
#include "event_loop.hpp"

namespace netlib {
class RequestHandler {
 public:
  RequestHandler(int32_t timeout = 1000*60): timeout_(timeout) {}


  void AsyncRecvRequest(EventLoop *el, int32_t fd, boost::shared_ptr<std::string> request);
  void AsyncSendResponse(EventLoop *el, int32_t fd, boost::shared_ptr<std::string> response);

  int32_t SyncRecvRequest(int32_t fd, boost::shared_ptr<std::string> request);
  int32_t SyncSendResponse(int32_t fd, boost::shared_ptr<std::string> response);

  // overwrite this function to handler request
  virtual void Process(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) = 0;

  int32_t GetTimeout() const { return timeout_; }
  void SetTimeout(int32_t timeout) { timeout_ = timeout; }

 private:
  int32_t timeout_;
};
}

#endif /* _REQUEST_HANDLER_H_ */

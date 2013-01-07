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

#ifndef _DISPATCH_HANDLER_H_
#define _DISPATCH_HANDLER_H_
#include <map>
#include <string>
#include <tr1/functional>
#include "request_handler.hpp"

namespace netlib {
std::string BuildHeader(const std::string &id);
std::string ParseHeader(const std::string &str);

class DispatchHandler: public RequestHandler {
 public:
  typedef std::tr1::function<void (boost::shared_ptr<std::string>, boost::shared_ptr<std::string>)> ProcessorType;

  DispatchHandler(int32_t timeout = -1): RequestHandler(timeout) {}
  bool AddProcessor(const std::string &id, const ProcessorType &processor);
  bool DeleteProcessor(const std::string &id);

  // implement the pure virtual function
  void Process(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response);
  virtual ~DispatchHandler();
 private:
  std::map<std::string, ProcessorType> processor_map_;
};
}

#endif /* _DISPATCH_HANDLER_H_ */




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
#include "dispatch_handler.hpp"
#include "buffer_io.hpp"
#include <glog/logging.h>

namespace netlib {
std::string BuildHeader(const std::string &id) {
  BufferIO io;
  io.WriteUInt8(id.length());
  io.WriteString(id);
  return io.GetBuffer();
}

std::string ParseHeader(const std::string &str) {
  BufferIO io;
  io.WriteBytes(str.c_str(), 1);
  uint8_t len = 0;
  std::string ret;
  if (io.ReadUInt8(&len) == 1 && str.length() > 1u+len) {
    ret = str.substr(1, len);
  } else {
    LOG(ERROR) << "failed to parse header";
  }
  return ret;
}

DispatchHandler::~DispatchHandler() {}

void DispatchHandler::Process(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) {
  std::string id = ParseHeader(*request);
  if (processor_map_.find(id) == processor_map_.end()) {
    LOG(ERROR) << "cannot find & execute processor: " << id;
    return;
  }
  processor_map_[id](request, response);
}

bool DispatchHandler::AddProcessor(const std::string &id, const ProcessorType &processor) {
  processor_map_[id] = processor;
  return true;
}

bool DispatchHandler::DeleteProcessor(const std::string &id) {
  std::map<std::string, ProcessorType>::iterator iter;
  iter = processor_map_.find(id);
  if (iter == processor_map_.end()) {
    LOG(ERROR) << "cannot find & delete processor: " << id;
    return false;
  }
  processor_map_.erase(iter);
  return true;
}

}

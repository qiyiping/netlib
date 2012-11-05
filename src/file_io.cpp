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
#include "file_io.hpp"

namespace netlib {
File::File(const std::string &path, uint32_t mode): file_(NULL) {
  Open(path, mode);
}

int32_t File::Open(const std::string &path, uint32_t mode) {
  if (IsOpen()) return RETURN_ERR;

  const char *str_mode = NULL;
  switch(mode) {
    case OPEN_READ:
      str_mode = "r";
      break;
    case OPEN_WRITE:
      str_mode = "w";
      break;
    case OPEN_APPEND:
      str_mode = "a";
      break;
    default:
      mode_ = OPEN_NONE;
      return RETURN_ERR;
  }

  file_ = fopen(path.c_str(), str_mode);
  if (!file_) {
    mode_ = OPEN_NONE;
    path_.clear();
    return RETURN_ERR;
  } else {
    mode_ = mode;
    path_ = path;
    return RETURN_OK;
  }
}

void File::Close() {
  if (file_) {
    fclose(file_);
    file_ = NULL;
    mode_ = OPEN_NONE;
    path_.clear();
  }
}

int32_t File::ReadBytes(void *buf, uint32_t size) {
  if (mode_ != OPEN_READ) {
    return RETURN_ERR;
  }
  return fread(buf, 1, size, file_);
}

int32_t File::WriteBytes(const void *buf, uint32_t size) {
  if (mode_ != OPEN_WRITE && mode_ != OPEN_APPEND) {
    return RETURN_ERR;
  }
  return fwrite(buf, 1, size, file_);
}

int32_t File::ReadString (std::string *str) {
  str->clear();
  int32_t ret = 0;
  char buf[1024*4];
  int32_t nbytes = ReadBytes(buf, 1024*4);
  while (nbytes) {
    ret += nbytes;
    str->append(buf, nbytes);
    nbytes = ReadBytes(buf, 1024*4);
  }
  return ret;
}

int32_t File::WriteString(const std::string &str) {
  return WriteBytes(str.c_str(), str.length());
}

}

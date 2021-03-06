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
#ifndef _FILE_IO_H_
#define _FILE_IO_H_
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "binary_io.hpp"

namespace netlib {
enum FileOpenMode {
  OPEN_NONE = 0,
  OPEN_READ = 1,
  OPEN_WRITE = 2,
  OPEN_APPEND = 4,
};

class File: public BinaryIO {
 public:
  File(): file_(NULL), mode_(OPEN_NONE) {}
  File(const std::string &path, uint32_t mode = OPEN_READ);

  int32_t Open(const std::string &path, uint32_t mode = OPEN_READ);
  void Close();
  bool IsOpen() const {
    return (file_ != NULL);
  }

  FILE *GetFile() { return file_; }
  std::string GetPath() const { return path_; }

  int32_t ReadBytes(void *buf, uint32_t size);
  int32_t WriteBytes(const void *buf, uint32_t size);

  int32_t ReadString (std::string *str);
  int32_t WriteString(const std::string &str);

  ~File() { Close(); }
 private:
  FILE *file_;
  std::string path_;
  uint32_t mode_;

  DISALLOW_COPY_AND_ASSIGN(File);
};
}

#endif /* _FILE_IO_H_ */

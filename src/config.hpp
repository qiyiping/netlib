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

#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <limits>
#include <cstddef>
#include <stdint.h>

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName)      \
  TypeName(const TypeName&);                    \
  void operator=(const TypeName&)

// A macro to check conditions statically
// It's similar to `BOOST_STATIC_ASSERT' whose implementation relies on
// sizeof(incomplete_type) to generate error message on compilation time.
template <bool expr> struct netlib_static_assert  {};
#define NETLIB_STATIC_ASSERT(expr, hint)                                \
  typedef netlib_static_assert<bool(expr)> hint[bool(expr)? 1:-1]

const int32_t RETURN_OK = 0;
const int32_t RETURN_ERR = -1;

const int8_t kInt8Min = std::numeric_limits<int8_t>::min();
const int16_t kInt16Min = std::numeric_limits<int16_t>::min();
const int32_t kInt32Min = std::numeric_limits<int32_t>::min();
const int64_t kInt64Min = std::numeric_limits<int64_t>::min();
const uint8_t kUInt8Min = std::numeric_limits<uint8_t>::min();
const uint16_t kUInt16Min = std::numeric_limits<uint16_t>::min();
const uint32_t kUInt32Min = std::numeric_limits<uint32_t>::min();
const uint64_t kUInt64Min = std::numeric_limits<uint64_t>::min();
const int8_t kInt8Max = std::numeric_limits<int8_t>::max();
const int16_t kInt16Max = std::numeric_limits<int16_t>::max();
const int32_t kInt32Max = std::numeric_limits<int32_t>::max();
const int64_t kInt64Max = std::numeric_limits<int64_t>::max();
const uint8_t kUInt8Max = std::numeric_limits<uint8_t>::max();
const uint16_t kUInt16Max = std::numeric_limits<uint16_t>::max();
const uint32_t kUInt32Max = std::numeric_limits<uint32_t>::max();
const uint64_t kUInt64Max = std::numeric_limits<uint64_t>::max();

#endif /* _CONFIG_H_ */

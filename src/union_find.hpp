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

#ifndef _UNION_FIND_H_
#define _UNION_FIND_H_
#include "config.hpp"
#include <boost/scoped_array.hpp>

namespace netlib {

/*
 * Union find data struct
 * @class UnionFind
 * Applications:
 * Dynamic connectivity
 */
class UnionFind {
 public:
  struct UFNode {
    int32_t size;
    int32_t parent;
  };

  UnionFind(int32_t n): nelements_(n), id_(new UFNode[n]) {
    for (int32_t i = 0; i < n; ++i) {
      id_[i].parent = i;
      id_[i].size = 1;
    }
  }

  int32_t GetRoot(int32_t p) const {
    if (id_[p].parent == p)
      return p;
    id_[p].parent = GetRoot(id_[p].parent);
    return id_[p].parent;
  }

  void Union(int32_t p, int32_t q) {
    int32_t proot = GetRoot(p);
    int32_t qroot = GetRoot(q);
    if (proot == qroot)
      return;
    if (id_[proot].size > id_[qroot].size) {
      id_[qroot].parent = proot;
      id_[proot].size += id_[qroot].size;
    } else {
      id_[proot].parent = qroot;
      id_[qroot].size += id_[proot].size;
    }
  }

  bool Connected(int32_t p, int32_t q) const {
    int32_t proot = GetRoot(p);
    int32_t qroot = GetRoot(q);
    return (proot == qroot);
  }

 private:
  int32_t nelements_;
  boost::scoped_array<UFNode> id_;

  DISALLOW_COPY_AND_ASSIGN(UnionFind);
};
}
#endif /* _UNION_FIND_H_ */

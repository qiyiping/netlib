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
#ifndef _FIXED_CAPACITY_HASH_MAP_H_
#define _FIXED_CAPACITY_HASH_MAP_H_

#include "config.hpp"
#include <utility>
#include "hash.hpp"
namespace netlib {
template <typename KeyType, typename ValueType>
struct HashNode {
  typedef std::pair<KeyType, ValueType> data_type;
  HashNode *next;
  data_type data;
};

template <typename KeyType, typename ValueType, typename HashFunc=hash<KeyType> >
class HashMap;

template <typename KeyType, typename ValueType, typename HashFunc>
struct HashMapIterator {
  typedef HashMap<KeyType, ValueType, HashFunc> hash_map;
  typedef HashMapIterator<KeyType, ValueType, HashFunc> iterator;
  typedef HashNode<KeyType, ValueType> node_type;
  typedef std::pair<KeyType, ValueType> data_type;
  hash_map *map;
  node_type *current;

  HashMapIterator() {}
  HashMapIterator(hash_map *m, node_type *n)
      :map(m), current(n) {}

  iterator &operator++() {
    node_type *old = current;
    current = current->next;
    if (!current) {
      std::size_t bucket_idx = map->get_index(old->data.first);
      while (!current && ++bucket_idx < map->capacity())
        current = map->buckets_[bucket_idx];
    }
    return *this;
  }

  iterator operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
  }

  bool operator==(const iterator &rhs) {
    return rhs.current == current;
  }

  bool operator!=(const iterator &rhs) {
    return rhs.current != current;
  }

  data_type &operator*() { return current->data; }
  data_type *operator->() { return &current->data; }
};

/*
 * The number of buckets is fixed.
 * Based on `HashMap' and `BitMutex',
 * we can implement entry level lock free thread safe hash map.
 * Usage:
 *     HashMap<std::string, int> h(1000);
 *     BitMutex mutex(h.capacity());
 *
 *     // lock entry
 *     {
 *       ScopedBitMutexLock lock(mutex, h.get_index("hello"));
 *       // do something with h["hello"];
 *     }
 */
template <typename KeyType, typename ValueType, typename HashFunc>
class HashMap {
 public:
  typedef KeyType key_type;
  typedef ValueType value_type;
  typedef HashFunc hasher_type;
  typedef HashNode<KeyType, ValueType> node_type;
  typedef std::size_t size_type;

  HashMap(size_type capacity)
      : capacity_(roundup(capacity)), size_(0) {
    buckets_ = new node_type*[capacity_];
    for (size_type i = 0; i < capacity_; ++i)
      buckets_[i] = NULL;
  }

  ~HashMap() {
    clear();
    delete[] buckets_;
  }

  size_type capacity() const { return capacity_; }
  size_type size() const { return size_; }
  bool empty() const { return size_ == 0; }

  typedef HashMapIterator<KeyType, ValueType, HashFunc> iterator;
  friend struct HashMapIterator<KeyType, ValueType, HashFunc>;

  iterator begin() {
    for (size_type i = 0; i < capacity_; ++i)
      if (buckets_[i])
        return iterator(this, buckets_[i]);
    return end();
  }

  iterator end() { return iterator(this, NULL); }

  iterator find(const key_type &k) {
    size_type bucket_idx = get_index(k);
    node_type *p = buckets_[bucket_idx];
    for (; p && p->data.first != k; p = p->next) {}
    return iterator(this, p);
  }

  void erase(const key_type &k) {
    size_type bucket_idx = get_index(k);
    node_type *p = buckets_[bucket_idx];
    if (p) {
      if (p->data.first == k) {
        buckets_[bucket_idx] = p->next;
        delete p;
        size_--;
      } else {
        for (; p->next && p->next->data.first != k; p = p->next) {}
        if (p->next) {
          node_type *q = p->next;
          p->next = q->next;
          delete q;
          size_--;
        }
      }
    }
  }

  void clear() {
    for (size_type i = 0; i < capacity_; ++i) {
      node_type *p = buckets_[i];
      while (p) {
        node_type *q = p->next;
        delete p;
        p = q;
      }
      buckets_[i] = NULL;
    }
    size_ = 0;
  }

  value_type &operator[](const key_type &k) {
    node_type *p = find_or_insert(k);
    return p->data.second;
  }

  size_type get_index(const key_type &k) {
    return hasher_(k) & capacity_;
  }
 private:
  const size_type capacity_;
  node_type **buckets_;
  hasher_type hasher_;
  size_type size_;
 private:
  size_type roundup(size_type capacity) {
    size_type c = 0;
    for (size_type i = 1; c < capacity; ++i) {
      c = (1 << i) - 1;
    }
    return c;
  }

  node_type *find_or_insert(const key_type &k) {
    size_type bucket_idx = get_index(k);
    node_type *p = buckets_[bucket_idx];
    for (; p && p->data.first != k; p = p->next) {}
    if (!p) {
      p = new node_type;
      p->data.first = k;
      p->next = buckets_[bucket_idx];
      buckets_[bucket_idx] = p;
      size_++;
    }
    return p;
  }
};
}
#endif /* _FIXED_CAPACITY_HASH_MAP_H_ */

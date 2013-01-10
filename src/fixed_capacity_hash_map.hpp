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

template <typename KeyType, typename ValueType>
struct HashNode {
  typedef std::pair<KeyType, ValueType> data_type;
  HashNode *next;
  data_type data;
};

template <typename KeyType, typename ValueType, typename HashFunc>
class HashMap;

template <typename KeyType, typename ValueType, typename HashFunc>
struct HashMapIterator {
  typedef HashMap<KeyType, ValueType, HashFunc> hash_map;
  typedef HashMapIterator<KeyType, ValueType, HashFunc> iterator;
  typedef HashNode<KeyType, ValueType> node_type;
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
        current = map.buckets_[bucket_idx];
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
 * @class HashMap
 * It's a fixed buckets hash map implementation
 */
template <typename KeyType, typename ValueType, typename HashFunc=hash<KeyType> >
class HashMap {
 public:
  typedef KeyType key_type;
  typedef ValueType value_type;
  typedef HashFunc hasher_type;
  typedef HashNode<KeyType, ValueType> node_type;
  typedef std::size_t size_type;

  HashMap(size_type capacity) {
    capacity_ = roundup(capacity);
    size_ = 0;
    buckets_ = new node_type*[capacity_];
    for (size_type i = 0; i < capacity_; ++i)
      buckets_[i] = NULL;
  }

  size_type capacity() const { return capacity_; }
  size_type size() const { return size_; }

  typedef HashMapIterator<KeyType, ValueType, HashFunc> iterator;
  friend struct HashMapIterator<KeyType, ValueType, HashFunc>;

  iterator begin() {
    for (size_type i = 0; i < capacity_; ++i)
      if (buckets_[i])
        return iterator(this, buckets_[i]);
  }

  iterator end() { return iterator(this, NULL); }

  iterator find(const key_type &k) {
    size_type bucket_idx = get_index(k);
    node_type *p = buckets_[bucket_idx];
    for (; p && p->data.first != k; p = p->next) {}
    return iterator(this, p);
  }

  void erase(const key_type &k) {}
  void clear() {}
  value_type &operator[](const key_type &k) {}
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

  size_type get_index(const key_type &k) {
    return hasher_(k) & capacity_;
  }
};
#endif /* _FIXED_CAPACITY_HASH_MAP_H_ */

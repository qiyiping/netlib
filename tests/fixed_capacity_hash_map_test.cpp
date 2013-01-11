#include "fixed_capacity_hash_map.hpp"
#include <iostream>
#include <map>
#include <ext/hash_map>
#include "time.hpp"
using namespace netlib;

int main(int argc, char *argv[]) {
  HashMap<std::string, int32_t> h(1000);
  h["hello"] = 1;
  h["world"] = 2;
  h["!"] = 123;
  HashMap<std::string, int32_t>::iterator it;
  for (it = h.begin(); it != h.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  std::cout << h.size() << std::endl;
  std::cout << h.capacity() << std::endl;
  std::cout << "--------------" << std::endl;

  h.erase("hello");
  for (it = h.begin(); it != h.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  std::cout << h.size() << std::endl;
  std::cout << h.capacity() << std::endl;
  std::cout << "--------------" << std::endl;

  std::cout << h["no"] << std::endl;
  it = h.find("no");
  it->second = 1111;
  // it->first = "big"; // compiler error! key is const!
  for (it = h.begin(); it != h.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  std::cout << h.size() << std::endl;
  std::cout << h.capacity() << std::endl;
  std::cout << "--------------" << std::endl;

  HashMap<std::string, int32_t>::const_iterator const_it;
  const_it = h.find("world");
  std::cout << const_it->second << std::endl;
  // const_it->second = 20; // compiler error! cannot change the value through const_iterator!
  const_it = h.find("hello");
  std::cout << (const_it == h.end()) << std::endl;
  const_it = h.find("0");
  std::cout << (const_it == h.end()) << std::endl;
  std::cout << h.size() << std::endl;
  std::cout << "--------------" << std::endl;
  for (const_it = h.begin(); const_it != h.end(); ++const_it) {
    std::cout << const_it->first << ":" << const_it->second << std::endl;
  }
  std::cout << h.size() << std::endl;
  std::cout << h.capacity() << std::endl;
  std::cout << "--------------" << std::endl;

  it = h.find("world");
  std::cout << it->second << std::endl;
  it = h.find("hello");
  std::cout << (it == h.end()) << std::endl;
  it = h.find("0");
  std::cout << (it == h.end()) << std::endl;
  std::cout << h.size() << std::endl;
  std::cout << "--------------" << std::endl;

  h.clear();
  for (it = h.begin(); it != h.end(); ++it) {
    std::cout << it->first << ":" << it->second << std::endl;
  }
  std::cout << h.size() << std::endl;
  std::cout << h.capacity() << std::endl;

  // performance compare with stl map/hash_map
  HashMap<int32_t, int32_t> h1(1000000);
  {
    Timer timer("h1 set: ");
    for (int i = 0; i < 500000; ++i) {
      h1[i] = 2*i;
    }
  }
  {
    Timer timer("h1 visit: ");
    HashMap<int32_t, int32_t>::iterator it;
    for (it = h1.begin(); it != h1.end(); ++it) {
      if (it->second != it->first*2) { std::cout << "error: " << it->first << "," << it->second << std::endl; }
    }
  }
  {
    Timer timer("h1 delete: ");
    for (int i = 0; i < 500000; ++i) {
      h1.erase(i);
    }
  }
  std::cout << h1.size() << std::endl;

  std::map<int32_t, int32_t> h2;
  {
    Timer timer("h2 set: ");
    for (int i = 0; i < 500000; ++i) {
      h2[i] = 2*i;
    }
  }
  {
    Timer timer("h2 visit: ");
    std::map<int32_t, int32_t>::iterator it;
    for (it = h2.begin(); it != h2.end(); ++it) {
      if (it->second != it->first*2) { std::cout << "error: " << it->first << "," << it->second << std::endl; }
    }
  }
  {
    Timer timer("h2 delete: ");
    for (int i = 0; i < 500000; ++i) {
      h2.erase(i);
    }
  }
  std::cout << h2.size() << std::endl;

  __gnu_cxx::hash_map<int32_t, int32_t> h3;
  {
    Timer timer("h3 set: ");
    for (int i = 0; i < 500000; ++i) {
      h3[i] = 2*i;
    }
  }
  {
    Timer timer("h3 visit: ");
    __gnu_cxx::hash_map<int32_t, int32_t>::iterator it;
    for (it = h3.begin(); it != h3.end(); ++it) {
      if (it->second != it->first*2) { std::cout << "error: " << it->first << "," << it->second << std::endl; }
    }
  }
  {
    Timer timer("h3 delete: ");
    for (int i = 0; i < 500000; ++i) {
      h3.erase(i);
    }
  }
  std::cout << h3.size() << std::endl;
  return 0;
}

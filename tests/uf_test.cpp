#include "union_find.hpp"
#include <glog/logging.h>

using namespace netlib;

int main(int argc, char *argv[]) {
  UnionFind uf(10000);
  uf.Union(1,100);
  uf.Union(2,99);
  uf.Union(1,2);
  uf.Union(3,5);
  CHECK(uf.Connected(100, 99));
  CHECK(uf.Connected(5, 3));
  return 0;
}

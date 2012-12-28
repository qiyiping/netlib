#include "thread_pool.hpp"
#include <iostream>
#include "time.hpp"

using namespace netlib;

void mycalc(int64_t i, int64_t j, int64_t *r) {
  *r = 0;
  while (i < j) *r += (i++);
}

int main(int argc, char *argv[]) {
  ThreadPool threadpool(60, 10000);
  const int32_t kSegs = 100;
  int64_t r[kSegs];
  int64_t step = 3100000;
  int64_t beg = 0;
  int64_t end = beg;
  int64_t t1 = GetMicroSeconds();
  for (int32_t i = 0; i < kSegs; ++i) {
    threadpool.AddTask(std::tr1::bind(mycalc, end, end+step, r+i));
    end += step;
  }
  threadpool.Join();
  int64_t r1 = 0;
  for (int32_t i = 0; i < kSegs; ++i) {
    r1 += r[i];
  }
  t1 = GetMicroSeconds() - t1;
  int64_t t2 = GetMicroSeconds();
  int64_t r2 = 0;
  while (beg < end) {
    r2 += (beg++);
  }
  t2 = GetMicroSeconds() - t2;
  std::cout << t1 << "\t" << r1 << std::endl
            << t2 << "\t" << r2 << std::endl;
  return 0;
}

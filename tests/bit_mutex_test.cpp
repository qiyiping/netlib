#include "thread.hpp"
#include "bit_mutex.hpp"
#include "mutex.hpp"
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "time.hpp"
#include <algorithm>
using namespace netlib;
using namespace std;
const int num_resource = 28000;
const int num_threads = 200;

#define BITMUTEXTEST
#define MUTEXTEST

#ifdef BITMUTEXTEST
BitMutex mu1(num_resource);
int arr1[num_resource];

class MyThread1: public Thread {
 public:
  MyThread1(): Thread(false) {
    idx = new int[num_resource];
    for (int i = 0; i < num_resource; ++i) {
      idx[i] = i;
    }
    random_shuffle(idx, idx+num_resource);
  }
  ~MyThread1() {
    delete[] idx;
  }
 protected:
  void Run() {
    for (int i = 0; i < num_resource; ++i) {
      ScopedBitMutexLock lock(mu1, idx[i], 5);
      arr1[idx[i]]++;
    }
  }

  int *idx;
};

#endif

#ifdef MUTEXTEST
// Mutex mu2[num_resource];
Mutex mu2;
int arr2[num_resource];

class MyThread2: public Thread {
 public:
  MyThread2(): Thread(false) {
    idx = new int[num_resource];
    for (int i = 0; i < num_resource; ++i) {
      idx[i] = i;
    }
    random_shuffle(idx, idx+num_resource);
  }
  ~MyThread2() {
    delete[] idx;
  }
 protected:
  void Run() {
    for (int i = 0; i < num_resource; ++i) {
      // ScopedMutexLock lock(mu2[idx[i]]);
      ScopedMutexLock lock(mu2);
      arr2[idx[i]]++;
    }
  }

  int *idx;
};
#endif

int main(int argc, char *argv[]) {
  srand(time(NULL));
#ifdef BITMUTEXTEST
  {
    MyThread1 t1[num_threads];
    Timer timer1("bit mutex: ");
    for (int i = 0; i < num_threads; ++i) {
      t1[i].Start();
    }
    for (int i = 0; i < num_threads; ++i) {
      t1[i].Join();
    }
    for (int i = 0; i < num_resource; ++i) {
      if (arr1[i] != num_threads)
        cout << arr1[i] << endl;
    }
  }
#endif
#ifdef MUTEXTEST
  {
    MyThread2 t2[num_threads];
    Timer timer2("mutex: ");
    for (int i = 0; i < num_threads; ++i) {
      t2[i].Start();
    }
    for (int i = 0; i < num_threads; ++i) {
      t2[i].Join();
    }
    for (int i = 0; i < num_resource; ++i) {
      if (arr2[i] != num_threads)
        cout << arr2[i] << endl;
    }
  }
#endif
  return 0;
}

#include "thread.hpp"
#include "mutex.hpp"
#include "time.hpp"
#include <queue>
#include <iostream>
using namespace netlib;
Mutex mu;
Cond cond(mu);
std::queue<int> myqueue;

class ConsumerThread:public Thread {
 public:
  ConsumerThread():Thread(false) {}
 protected:
  void Run() {
    while (true) {
      ScopedMutexLock lock(mu);
      if (myqueue.empty()) {
        cond.Wait();
      }
      int val = myqueue.front();
      std::cout << "consumer thread: " << val << " poped"<< std::endl;
      myqueue.pop();
      if (myqueue.size() < 2) {
        cond.Notify();
      }
      if (val > 100)
        break;
    }
  }
};

class ProducerThread:public Thread {
 public:
  ProducerThread():Thread(false), val_(0) {}
 protected:
  int val_;
  void Run() {
    while(true) {
      ScopedMutexLock lock(mu);
      if (myqueue.size() >= 5) {
        cond.Wait();
      }
      myqueue.push(val_++);
      std::cout << "producer thread: " << val_-1 << " pushed" << std::endl;
      if (!myqueue.empty()) {
        cond.Notify();
      }
      if (val_ > 101) {
        break;
      }
    }
  }
};

int main(int argc, char *argv[]) {
  ProducerThread p;
  ConsumerThread c;
  c.Start();
  p.Start();
  c.Join();
  p.Join();
  return 0;
}

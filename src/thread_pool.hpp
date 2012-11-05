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
#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_
#include <tr1/functional>
#include <unistd.h>
#include <boost/shared_ptr.hpp>
#include "thread.hpp"
#include "sync_queue.hpp"

namespace netlib {
typedef std::tr1::function<void (void)> TaskCallback;
typedef SyncQueue<TaskCallback> TaskQueue;

class Worker:public Thread {
 public:
  Worker(TaskQueue *task_queue):Thread(false),
                                task_queue_(task_queue),
                                stop_(false),
                                sleep_usecs_(0) {}
  void Stop() { stop_ = true; }
 protected:
  void Run();
 private:
  TaskQueue *task_queue_;
  volatile bool stop_;
  int32_t sleep_usecs_;
  static const int32_t kMaxSleepUsecs = 7000;
};

inline
void Worker::Run() {
  TaskCallback callback;
  while (!stop_) {
    if (task_queue_->TryPop(&callback)) {
      sleep_usecs_ /= 2;
      callback();
    } else {
      usleep(sleep_usecs_);
      sleep_usecs_ = sleep_usecs_+1 < kMaxSleepUsecs ?
                     sleep_usecs_+1 : kMaxSleepUsecs;
    }
  }
}

class ThreadPool {
 public:
  ThreadPool(uint32_t nworkers, uint32_t queue_limits);
  void AddTask(const TaskCallback &task) { task_queue_->Push(task); }
  // wait for all the tasks to be finished
  void WaitAllTasks();
  // stop all the workers even thought there are remaining tasks in the task queue
  void Join();
 private:
  std::vector<boost::shared_ptr<Worker> > workers_;
  boost::scoped_ptr<TaskQueue> task_queue_;
};

inline
ThreadPool::ThreadPool(uint32_t nworkers, uint32_t queue_limits) {
  task_queue_.reset(new TaskQueue(queue_limits));
  for (uint32_t i = 0; i < nworkers; ++i) {
    boost::shared_ptr<Worker> worker(new Worker(task_queue_.get()));
    workers_.push_back(worker);
  }
  for (uint32_t i = 0; i < nworkers; ++i) {
    workers_[i]->Start();
  }
}

inline
void ThreadPool::WaitAllTasks() {
  while (!task_queue_->Empty()) {
    usleep(10);
  }
  Join();
}

inline
void ThreadPool::Join() {
  for (uint32_t i = 0; i < workers_.size(); ++i) {
    workers_[i]->Stop();
  }
  for (uint32_t i = 0; i < workers_.size(); ++i) {
    workers_[i]->Join();
  }
}

}

#endif /* _THREAD_POOL_H_ */

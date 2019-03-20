/*-------------------------------*\
Copyright 2019 Kiyo Matsui
KiyosGameEngine v0.74
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_THREADPOOL
#define KGE_THREADPOOL

#include <atomic>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

namespace kge {

template <unsigned int N = 4> 
class threadPool {
public:
  threadPool() {
    for (unsigned i = 0; i < N; ++i) {
      
      threads.emplace_back([=]() {
        for (;;) {
          m.lock();
          if (jobQueue.empty()) {
            m.unlock();
            if (killFlag)
              break;
            std::this_thread::yield();
          } else {
            std::function<void()> job = jobQueue.front();
            jobQueue.pop();
            m.unlock();
            job();
          }
        }
      });
    }
  }

  ~threadPool() {
    killFlag.exchange(true);
    for (auto &n : threads) {
      n.join();
    }
  }

  template <typename FunctionType> void submitJob(FunctionType functor) {
    std::lock_guard<std::mutex> lock(m);
    jobQueue.push(std::function<void()>(functor));
  }
  
  bool tryJob() {
    m.lock();
    if (jobQueue.empty()) {
      m.unlock();
      return false;
    } else {
      std::function<void()> job = jobQueue.front();
      jobQueue.pop();
      m.unlock();
      job();
      return true;
    }
  }

private:
  std::queue<std::function<void()>> jobQueue;
  std::vector<std::thread> threads;
  mutable std::mutex m;
  std::atomic_bool killFlag = false;
};

} // namespace kge

#endif

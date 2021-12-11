/*-------------------------------*\
Copyright 2021 Kiyo Matsui
KiyosGameEngine v2.0
Apache License
Version 2.0, January 2004
http://www.apache.org/licenses/
\*-------------------------------*/

#ifndef KGE_THREADPOOL
#define KGE_THREADPOOL

#include <atomic>
#include <functional>
#include <mutex>
#include <condition_variable>
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
          std::function<void()> job;
          {
            std::unique_lock<std::mutex> lk(m);
            cv.wait(lk, [this]{return !(jobQueue.empty()) || killFlag.load(); });
            if (killFlag.load()) {
              break;
            }
            job = jobQueue.front();
            jobQueue.pop();
          }
          job();
        }
      });
    }
  }

  threadPool(const threadPool& other) = delete;
  threadPool& operator=(const threadPool& other) = delete;
  threadPool(threadPool&& other) noexcept = delete;
  threadPool& operator=(threadPool&& other) noexcept = delete;

  ~threadPool() {
    killFlag.exchange(true);
    cv.notify_all();
    for (auto& n : threads) {
      n.join();
    }
  }

  template <typename FunctionType>
  void submitJob(FunctionType functor) {
    {
      std::lock_guard<std::mutex> lock(m);
      jobQueue.push(std::function<void()>(functor));
    }
    cv.notify_one();
  }

  bool tryJob() {
    std::function<void()> job;
    {
      std::lock_guard<std::mutex> lock(m);
      if (!jobQueue.empty()) {
        job = jobQueue.front();
        jobQueue.pop();
      } else {
        return false;
      }
    }
    job();
    return true;
  }

 private:
  std::queue<std::function<void()>> jobQueue;
  std::vector<std::thread> threads;
  mutable std::mutex m;
  std::condition_variable cv;
  std::atomic_bool killFlag{false};
};

}  // namespace kge

#endif

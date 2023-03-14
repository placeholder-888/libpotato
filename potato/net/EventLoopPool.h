#ifndef POTATO_POTATO_NET_EVENTLOOPPOOL_H_
#define POTATO_POTATO_NET_EVENTLOOPPOOL_H_

#include "potato/thread/Mutex.h"
#include "potato/utils/NonCopyable.h"

#include <condition_variable>
#include <memory>
#include <thread>
#include <vector>

namespace potato {

class EventLoop;

class EventLoopPool : NonCopyable {
public:
  EventLoopPool(EventLoop *mainLoop, bool etMode = false);
  ~EventLoopPool();
  void setThreadNum(size_t threadNums);
  void start();

  EventLoop *getNextLoop();

private:
  void threadFunc();
  EventLoop *mainLoop_;
  bool etMode_;
  std::atomic_bool starting_{false};
  size_t threadNums_{0};
  size_t next_{0};
  Mutex lock_;
  std::condition_variable cond_;
  std::vector<std::unique_ptr<std::thread>> threads_;
  std::vector<EventLoop *> loops_;
};

} // namespace potato

#endif // POTATO_POTATO_NET_EVENTLOOPPOOL_H_

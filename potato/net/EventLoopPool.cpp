#include "potato/net/EventLoopPool.h"
#include "potato/log/Logger.h"
#include "potato/net/EventLoop.h"
#include <cassert>

using potato::EventLoopPool;

EventLoopPool::EventLoopPool(EventLoop *mainLoop, bool etMode)
    : mainLoop_(mainLoop), etMode_(etMode) {}

void EventLoopPool::setThreadNum(size_t threadNums) {
  threadNums_ = threadNums;
}

EventLoopPool::~EventLoopPool() {
  for (auto &loop : loops_) {
    loop->stopLoop();
  }
  for (auto &thread : threads_) {
    thread->join();
  }
}

void EventLoopPool::start() {
  for (size_t i = 0; i < threadNums_; ++i) {
    threads_.emplace_back(new std::thread(&EventLoopPool::threadFunc, this));
  }
  UniqueLock guard(lock_);
  cond_.wait(guard.getStdUniqueLock(),
             [this] { return loops_.size() == threadNums_; });
  starting_ = true;
}

void EventLoopPool::threadFunc() {
  EventLoop loop(etMode_);
  {
    MutexLockGuard guard(lock_);
    loops_.emplace_back(&loop);
    cond_.notify_one();
  }
  loop.startLoop();
}

potato::EventLoop *EventLoopPool::getNextLoop() {
  EventLoop *loop = mainLoop_;
  if (!loops_.empty()) {
    loop = loops_[next_++];
    if (next_ >= loops_.size())
      next_ = 0;
  }
  return loop;
}

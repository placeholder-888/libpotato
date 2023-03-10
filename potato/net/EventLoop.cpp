#include "potato/net/EventLoop.h"
#include "potato/log/Logger.h"
#include "potato/net/Socket.h"
#include <cassert>

using potato::EventLoop;
using potato::Socket;

namespace {

std::pair<Socket, Socket> createWakeupSocket() {
  socket_t socket[2];
  if (potato::socketPair(socket) < 0) {
    LOG_FATAL("Failed to create socket pair error:%s",
              potato::strError(perrno).c_str());
    abort();
  }
  potato::setNonBlock(socket[0]);
  potato::setNonBlock(socket[1]);
  return std::make_pair(Socket(socket[0]), Socket(socket[1]));
}

} // namespace

EventLoop::EventLoop(bool etMode)
    : threadId_(std::this_thread::get_id()), etMode_(etMode), ioWatcher_(this),
      wakeupSocket_(createWakeupSocket()),
      wakeupEvent_(wakeupSocket_.second.getPlatformSocket(), &ioWatcher_) {
  wakeupEvent_.setReadCallback([this]() {
    int dummy{};
    do {
      auto size = wakeupSocket_.second.read(&dummy, sizeof(dummy));
      if (size != sizeof(dummy)) {
        if (perrno != PAGAIN) {
          LOG_ERROR("error at read wakeup socket");
        }
        break;
      }
    } while (etMode_);
  });
}

EventLoop::~EventLoop() {
  wakeupEvent_.removeThisEvent();
  if (looping_) {
    stopLoop();
  }
}

void EventLoop::startLoop() {
  assert(!looping_);
  looping_ = true;
  wakeupEvent_.expectReading();
  while (looping_) {
    // TODO 处理定时器事件
    ioWatcher_.handleIOEvent(1000);
    callPendingFunctors();
  }
}

void EventLoop::stopLoop() {
  assert(looping_);
  looping_ = false;
  if (!inLoopThread()) {
    wakeup();
  }
}

void EventLoop::wakeup() const {
  int dummy = 1;
  auto size = wakeupSocket_.first.write(&dummy, sizeof(dummy));
  if (size != sizeof(dummy)) {
    LOG_ERROR("error at write wakeup socket");
  }
}

void EventLoop::callPendingFunctors() {
  std::vector<Functor> functors;
  {
    MutexLockGuard lock(mutex_);
    functors.swap(pendingFunctors_);
  }
  for (const auto &functor : functors) {
    functor();
  }
}

void EventLoop::runInLoop(Functor func) {
  if (inLoopThread()) {
    func();
  } else {
    MutexLockGuard lock(mutex_);
    pendingFunctors_.push_back(std::move(func));
    wakeup();
  }
}

#ifndef POTATO_POTATO_NET_EVENTLOOP_H_
#define POTATO_POTATO_NET_EVENTLOOP_H_

#include "potato/net/IOWatcher.h"
#include "potato/net/Socket.h"
#include "potato/thread/Mutex.h"
#include "potato/utils/NonCopyable.h"

#include <atomic>
#include <functional>
#include <thread>

namespace potato {

class IOWatcher;

class EventLoop : NonCopyable {
public:
  using Functor = std::function<void()>;

  explicit EventLoop(bool etMode = false);
  ~EventLoop();

  void startLoop();
  void stopLoop();

  bool isLooping() const { return looping_; }
  bool etMode() const { return etMode_; }

  bool inLoopThread() const { return threadId_ == std::this_thread::get_id(); }

  void runInLoop(Functor func);

  IOWatcher *watcher() { return &ioWatcher_; }

private:
  void wakeup() const;
  void callPendingFunctors();

  const std::thread::id threadId_;
  bool etMode_;
  IOWatcher ioWatcher_;
  std::pair<Socket, Socket> wakeupSocket_;
  IOEvent wakeupEvent_;
  std::atomic_bool looping_{false};
  Mutex mutex_;
  std::vector<Functor> pendingFunctors_ GUARDED_BY(mutex_);
};

} // namespace potato

#endif // POTATO_POTATO_NET_EVENTLOOP_H_

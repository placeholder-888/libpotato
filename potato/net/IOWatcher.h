#ifndef POTATO_POTATO_NET_IOWATCHER_H_
#define POTATO_POTATO_NET_IOWATCHER_H_

#include "potato/net/SocketCommon.h"
#include "potato/utils/NonCopyable.h"
#include <cassert>
#include <functional>
#include <vector>

#ifdef PLATFORM_WINDOWS
#define EPollHolder HANDLE
#define INVALID_HOLDER nullptr
#include "wepoll/wepoll.h"
#else
#define EPollHolder int
#define INVALID_HOLDER (-1)
#define epoll_close(x) ::close(x)
#include <sys/epoll.h>
#endif

namespace potato {

class IOEvent;
class EventLoop;

class IOWatcher : NonCopyable {
public:
  IOWatcher(EventLoop *loop);
  ~IOWatcher();
  void handleIOEvent(int timeoutMs);
  void enableIOReading(IOEvent *event);
  void enableIOWriting(IOEvent *event);
  void disableIOWriting(IOEvent *event);
  void removeIOEvent(IOEvent *event);

private:
  void addNewIOEvent(IOEvent *event);
  void modifyIOEvent(IOEvent *event);
  void deleteIOEvent(IOEvent *event);

  EPollHolder holder_{};
  EventLoop *ownerLoop_;
  std::vector<struct epoll_event> events_;
};

class IOEvent : NonCopyable {

  friend class IOWatcher;

public:
  using Callback = std::function<void()>;
  explicit IOEvent(socket_t socket, IOWatcher *watcher)
      : socket_(socket), watcher_(watcher) {}

  void setReadCallback(Callback callback) {
    readCallback_ = std::move(callback);
  }
  void setWriteCallback(Callback callback) {
    writeCallback_ = std::move(callback);
  }

  void setCloseCallback(Callback callback) {
    closeCallback_ = std::move(callback);
  }

  void expectReading() { watcher_->enableIOReading(this); }
  void expectWriting() { watcher_->enableIOWriting(this); }
  void ignoreWriting() { watcher_->disableIOWriting(this); }
  void removeThisEvent() { watcher_->removeIOEvent(this); }

  bool inEpoll_{false};

private:
  socket_t socket_;
  IOWatcher *watcher_;
  Callback readCallback_;
  Callback writeCallback_;
  Callback closeCallback_;
  uint32_t events_{0};
};

} // namespace potato

#endif // POTATO_POTATO_NET_IOWATCHER_H_

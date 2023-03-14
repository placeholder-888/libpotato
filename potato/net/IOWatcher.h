#ifndef POTATO_POTATO_NET_IOWATCHER_H_
#define POTATO_POTATO_NET_IOWATCHER_H_

#include "potato/net/SocketCommon.h"
#include "potato/utils/NonCopyable.h"
#include <atomic>
#include <cassert>
#include <functional>
#include <vector>

#ifdef PLATFORM_WINDOWS
#define EPollHolder HANDLE
#define INVALID_HOLDER nullptr
#include "3rdparty//wepoll/wepoll.h"
#else
#define EPollHolder int
#define INVALID_HOLDER (-1)
#define epoll_close(x) ::close(x)
#include <sys/epoll.h>
#endif

namespace potato {

class IOEvent;

class IOWatcher : NonCopyable {
public:
  explicit IOWatcher(bool etMode = false);
  ~IOWatcher();
  void handleIOEvent(int timeoutMs);
  void enableIOReading(IOEvent *event);
  void enableIOWriting(IOEvent *event);
  void disableIOWriting(IOEvent *event);
  void removeIOEvent(IOEvent *event);

  bool etMode() const { return etMode_; }

private:
  void addNewIOEvent(IOEvent *event);
  void modifyIOEvent(IOEvent *event);
  void deleteIOEvent(IOEvent *event);

  EPollHolder holder_{};
  std::vector<struct epoll_event> events_;
  bool etMode_{false};
};

class IOEvent : NonCopyable {

  friend class IOWatcher;

public:
  using Callback = std::function<void()>;
  explicit IOEvent(IOWatcher *watcher, socket_t socket)
      : watcher_(watcher), socket_(socket), etMode_(watcher->etMode()) {}

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
  void expectWriting() {
    if (isWriting_)
      return;
    watcher_->enableIOWriting(this);
    isWriting_ = true;
  }
  void ignoreWriting() {
    if (!isWriting_)
      return;
    watcher_->disableIOWriting(this);
    isWriting_ = false;
  }
  void removeThisEvent() { watcher_->removeIOEvent(this); }

  bool isWriting() const { return isWriting_; }

  bool inEpoll_{false};

  bool etMode() const { return etMode_; }

private:
  IOWatcher *watcher_;
  socket_t socket_;
  std::atomic_bool isWriting_{false};
  bool etMode_{false};
  Callback readCallback_;
  Callback writeCallback_;
  Callback closeCallback_;
  uint32_t events_{0};
};

} // namespace potato

#endif // POTATO_POTATO_NET_IOWATCHER_H_

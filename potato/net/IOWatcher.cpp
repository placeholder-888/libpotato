#include "potato/net/IOWatcher.h"
#include "potato/log/Logger.h"
#include "potato/net/SocketCommon.h"

using potato::IOWatcher;

IOWatcher::IOWatcher(bool etMode) : events_(16), etMode_(etMode) {
  holder_ = epoll_create(1);
  if (holder_ == INVALID_HOLDER) {
    LOG_FATAL("IOWatcher::IOWatcher() error at epoll_create");
    abort();
  }
#ifdef PLATFORM_WINDOWS
  etMode_ = false;
#endif
}

IOWatcher::~IOWatcher() { epoll_close(holder_); }

void IOWatcher::handleIOEvent(int timeoutMs) {
  int numEvents = epoll_wait(holder_, events_.data(),
                             static_cast<int>(events_.size()), timeoutMs);
  LOG_DEBUG("epoll_wait return {}", numEvents);
  if (numEvents < 0) {
    LOG_ERROR("IOWatcher::handleIOEvent() error at epoll_wait");
    return;
  }
  auto numEventsSize = static_cast<size_t>(numEvents);
  for (size_t i = 0; i < numEventsSize; ++i) {
    auto *event = static_cast<IOEvent *>(events_[i].data.ptr);
    auto ev = events_[i].events;
    if (ev & (EPOLLHUP | EPOLLRDHUP)) {
      if (event->closeCallback_)
        event->closeCallback_();
      continue;
    } else if (ev & (EPOLLIN | EPOLLPRI)) {
      if (event->readCallback_)
        event->readCallback_();
    } else if (ev & EPOLLOUT) {
      if (event->writeCallback_)
        event->writeCallback_();
    }
  }
  if (numEventsSize == events_.size()) {
    events_.resize(numEventsSize << 1);
  }
}

void IOWatcher::addNewIOEvent(IOEvent *event) {
  assert(event->watcher_ == this);
  assert(event->inEpoll_ == false);
  struct epoll_event ev {};
  ev.data.ptr = event;
  ev.events = event->events_;
#ifdef PLATFORM_LINUX
  if (etMode_) {
    ev.events |= EPOLLET;
  }
#endif
  if (epoll_ctl(holder_, EPOLL_CTL_ADD, event->socket_, &ev) < 0) {
    LOG_FATAL("IOWatcher::addNewIOEvent() error at epoll_ctl");
    abort();
  }
  event->inEpoll_ = true;
}

void IOWatcher::modifyIOEvent(IOEvent *event) {
  assert(event->watcher_ == this);
  assert(event->inEpoll_ == true);
  struct epoll_event ev {};
  ev.data.ptr = event;
  ev.events = event->events_;
#ifdef PLATFORM_LINUX
  if (etMode_) {
    ev.events |= EPOLLET;
  }
#endif
  if (epoll_ctl(holder_, EPOLL_CTL_MOD, event->socket_, &ev) < 0) {
    LOG_FATAL("IOWatcher::modifyIOEvent() error at epoll_ctl");
    abort();
  }
}

void IOWatcher::deleteIOEvent(IOEvent *event) {
  assert(event->socket_ != INVALID_SOCKET);
  assert(event->watcher_ == this);
  assert(event->inEpoll_ == true);
  if (epoll_ctl(holder_, EPOLL_CTL_DEL, event->socket_, nullptr) < 0) {
    LOG_FATAL("IOWatcher::removeIOEvent() error at epoll_ctl");
    abort();
  }
  event->inEpoll_ = false;
}

void IOWatcher::enableIOReading(IOEvent *event) {
  assert(event->events_ == 0);
  assert(event->inEpoll_ == false);
  event->events_ |= (EPOLLIN | EPOLLPRI);
  addNewIOEvent(event);
}

void IOWatcher::enableIOWriting(IOEvent *event) {
  assert(!event->isWriting_);
  assert(event->inEpoll_ == true);
  assert(event->events_ & EPOLLIN);
  event->events_ |= EPOLLOUT;
  modifyIOEvent(event);
}

void IOWatcher::disableIOWriting(IOEvent *event) {
  assert(event->isWriting_);
  assert(event->inEpoll_ == true);
  assert(event->events_ & EPOLLIN);
  event->events_ &= ~EPOLLOUT;
  modifyIOEvent(event);
}

void IOWatcher::removeIOEvent(IOEvent *event) {
  assert(event->inEpoll_ == true);
  deleteIOEvent(event);
  event->events_ = 0;
}

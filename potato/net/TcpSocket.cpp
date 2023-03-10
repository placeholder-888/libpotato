#include "potato/net/TcpSocket.h"
#include "potato/log/Logger.h"
#include "potato/net/EventLoop.h"
#include "potato/net/IOWatcher.h"

using potato::TcpSocket;

TcpSocket::TcpSocket(socket_t sock, const IpAddress &peer)
    : Socket(sock), peer_(peer), state_(kConnected) {}

TcpSocket::~TcpSocket() { safeClose(); }

void TcpSocket::attachToLoop(EventLoop *loop) {
  assert(ownerLoop_ == nullptr);
  ownerLoop_ = loop;
  event_.reset(new IOEvent(getPlatformSocket(), ownerLoop_.load()->watcher()));
  event_->setReadCallback(std::bind(&TcpSocket::handleRead, this));
  event_->setWriteCallback(std::bind(&TcpSocket::handleWrite, this));
  event_->setCloseCallback(std::bind(&TcpSocket::close, this));
  auto guard(shared_from_this());
  if (connectedCallback_)
    connectedCallback_(shared_from_this());
  ownerLoop_.load()->runInLoop([this, guard]() { event_->expectReading(); });
}

void TcpSocket::expectWriting() {
  assert(ownerLoop_);
  auto guard(shared_from_this());
  ownerLoop_.load()->runInLoop([this, guard]() { event_->expectWriting(); });
}

void TcpSocket::ignoreWriting() {
  assert(ownerLoop_);
  auto guard(shared_from_this());
  ownerLoop_.load()->runInLoop([this, guard]() { event_->ignoreWriting(); });
}

void TcpSocket::close() {
  if (state_ == kConnected) {
    state_ = kDisconnected;
    auto loop = ownerLoop_.load();
    safeClose();
    if (!loop)
      return;
    auto guard(shared_from_this());
    loop->runInLoop([this, guard]() {
      if (disconnectedCallback_)
        disconnectedCallback_(guard);
    });
  }
}

void TcpSocket::handleRead() {
  if (state_ != kConnected)
    return;
  if (readyReadCallback_)
    readyReadCallback_(shared_from_this());
}
void TcpSocket::handleWrite() {
  if (state_ != kConnected)
    return;
  if (readyWriteCallback_)
    readyWriteCallback_(shared_from_this());
}

void TcpSocket::safeClose() {
  if (ownerLoop_) {
    isOpen_ = false;
    event_->removeThisEvent();
    potato::close(getPlatformSocket());
    ownerLoop_ = nullptr;
  }
  state_ = kDisconnected;
}

#include "potato/net/Acceptor.h"
#include "potato/log/Logger.h"
#include "potato/net/EventLoop.h"
#include "potato/net/IOWatcher.h"
#include "potato/net/TcpSocket.h"

using potato::Acceptor;

Acceptor::Acceptor(EventLoop *loop, const IpAddress &address)
    : listenSocket_(address.ipv6()), hostAddress_(address), loop_(loop),
      ioEvent_(
          new IOEvent(listenSocket_.getPlatformSocket(), loop_->watcher())) {
  ioEvent_->setReadCallback([this]() { handleAccept(); });
  listenSocket_.bind(address);
#ifdef PLATFORM_LINUX
  idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
  if (idleFd_ < 0) {
    LOG_ERROR("Acceptor::Acceptor() Failed to open /dev/null");
  }
#endif
}

Acceptor::~Acceptor() {
#ifdef PLATFORM_LINUX
  if (idleFd_ >= 0) {
    ::close(idleFd_);
  }
#endif
  ioEvent_->removeThisEvent();
}

void Acceptor::listen() {
  listenSocket_.listen();
  loop_->runInLoop([this] { ioEvent_->expectReading(); });
}

void Acceptor::handleAccept() {
  do {
    auto p = listenSocket_.accept();
    if (p.first == INVALID_SOCKET) {
#ifdef PLATFORM_LINUX
      if (perrno == EMFILE) {
        ::close(idleFd_);
        idleFd_ = ::accept(listenSocket_.getPlatformSocket(), nullptr, nullptr);
        if (perrno == EAGAIN) {
          idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
          break;
        }
        ::close(idleFd_);
        idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
      } else if (perrno != PAGAIN) {
        LOG_ERROR("Acceptor::handleAccept() accept error %s",
                  potato::strError(perrno).c_str());
      } else {
        break;
      }
#else
      if (perrno != PAGAIN) {
        LOG_ERROR("Acceptor::handleAccept() accept error %s",
                  potato::strError(perrno).c_str());
      } else {
        break;
      }
#endif
    } else {
      if (newConnectionCallback_ && potato::setNonBlock(p.first) == 0) {
        newConnectionCallback_(TcpSocket::newTcpConnection(p.first, p.second));
      } else {
        LOG_WARN("Acceptor::handleAccept() newConnectionCallback_ is nullptr");
      }
    }
  } while (loop_->etMode());
}

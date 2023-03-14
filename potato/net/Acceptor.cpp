#include "potato/net/Acceptor.h"
#include "potato/log/Logger.h"
#include "potato/net/EventLoop.h"
#include "potato/net/IOWatcher.h"

using potato::Acceptor;

Acceptor::Acceptor(EventLoop *loop, const IpAddress &address)
    : listenSocket_(address.ipv6() ? Socket::kTcpSocket6 : Socket::kTcpSocket,
                    true),
      hostAddress_(address), loop_(loop),
      ioEvent_(
          new IOEvent(loop_->watcher(), listenSocket_.getPlatformSocket())) {
  ioEvent_->setReadCallback([this]() { handleAccept(); });
  listenSocket_.setReuseAddr(true);
  listenSocket_.setReusePort(true);
  listenSocket_.bind(address);
  listenSocket_.setNonBlock();
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
  ioEvent_->expectReading();
  listening_ = true;
  LOG_INFO("Acceptor::listen() processId:{} listen on {}", getProcessId(),
           hostAddress_.IpPort());
}

void Acceptor::handleAccept() {
  do {
    Socket::SocketPtr ptr = listenSocket_.accept();
    if (!ptr) {
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
        LOG_ERROR("Acceptor::handleAccept() accept error {}",
                  potato::strError(perrno));
      } else {
        break;
      }
#else
      if (perrno != PAGAIN) {
        LOG_ERROR("Acceptor::handleAccept() accept error {}",
                  potato::strError(perrno));
      } else {
        break;
      }
#endif
    } else {
      ptr->setNonBlock();
      if (newConnectionCallback_) {
        newConnectionCallback_(std::move(ptr));
      } else {
        LOG_WARN("Acceptor::handleAccept() newConnectionCallback_ is not set");
        ptr->close();
      }
    }
  } while (true);
}

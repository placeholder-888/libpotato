#include "potato/net/TcpConnection.h"
#include "potato/log/Logger.h"
#include "potato/net/EventLoop.h"
#include "potato/net/IOWatcher.h"

using potato::TcpConnection;

TcpConnection::TcpConnection(int index, const IpAddress &local)
    : index_(index), localAddress_(local), loop_(nullptr) {}

TcpConnection::~TcpConnection() {
  assert(!ioEvent_);
  assert(!socket_);
}

void TcpConnection::bindSocket(SocketPtr socket, EventLoop *loop) {
  assert(!loop_);
  assert(!socket_);
  assert(!ioEvent_);
  assert(socket->valid() && socket->isConnected());
  loop_ = loop;
  ioEvent_.reset(new IOEvent(loop->watcher(), socket->getPlatformSocket()));
  ioEvent_->setReadCallback(std::bind(&TcpConnection::handleReadEvent, this));
  ioEvent_->setWriteCallback(std::bind(&TcpConnection::handleWriteEvent, this));
  ioEvent_->setCloseCallback(std::bind(&TcpConnection::handleCloseEvent, this));
  socket_ = std::move(socket);
  loop->runInLoop([this]() {
    ioEvent_->expectReading();
    if (connectionCallback_)
      connectionCallback_(this);
  });
}

void TcpConnection::reset() {
  assert(ioEvent_);
  assert(socket_);
  assert(loop_);
  if (connectionCallback_)
    connectionCallback_(this);
  loop_ = nullptr;
  ioEvent_.reset();
  socket_.reset();
}

void TcpConnection::handleCloseEvent() {
  if (!socket_)
    return;
  assert(ioEvent_);
  assert(socket_);
  assert(loop_);
  if (socket_->isConnected()) {
    ioEvent_->removeThisEvent();
    socket_->close();
    if (destroyCallback_)
      destroyCallback_(this);
  }
}

void TcpConnection::close() {
  if (!socket_)
    return;
  assert(ioEvent_);
  assert(socket_);
  assert(loop_);
  loop_->runInLoop([this]() { handleCloseEvent(); });
}

void TcpConnection::handleReadEvent() {
  if (!socket_)
    return;
  assert(ioEvent_);
  assert(socket_);
  if (!socket_->isConnected())
    return;
  do {
    auto n =
        socket_->read(readBuffer_.beginWrite(), readBuffer_.writeableBytes());
    if (n > 0) {
      readBuffer_.hasWritten(static_cast<size_t>(n));
      if (readBuffer_.writeableBytes() < 4096) {
        readBuffer_.expandBuffer(4096);
      }
      if (messageCallback_) {
        messageCallback_(this);
      }
    } else if (n == 0) {
      handleCloseEvent();
      break;
    } else {
      if (perrno != PAGAIN) {
        LOG_ERROR("TcpConnection::handleReadEvent() read error");
        handleCloseEvent();
      }
      break;
    }
  } while (ioEvent_->etMode());
}

void TcpConnection::handleWriteEvent() {
  if (!socket_)
    return;
  assert(ioEvent_);
  assert(socket_);
  if (!socket_->isConnected())
    return;
  assert(ioEvent_->isWriting());
  if (writeBuffer_.readableBytes() > 0) {
    auto n = socket_->write(writeBuffer_.peek(), writeBuffer_.readableBytes());
    if (n > 0) {
      writeBuffer_.retrieve(static_cast<size_t>(n));
      if (writeBuffer_.readableBytes() == 0) {
        ioEvent_->ignoreWriting();
        if (writeCompleteCallback_) {
          writeCompleteCallback_(this);
        }
      }
    } else {
      if (perrno != PAGAIN) {
        LOG_ERROR("TcpConnection::handleWriteEvent() write error");
        handleCloseEvent();
      } else {
        ioEvent_->expectWriting();
      }
    }
  }
  if (writeBuffer_.readableBytes() == 0) {
    ioEvent_->ignoreWriting();
  }
}

void TcpConnection::send(StringSlice msg) { write(msg); }

void TcpConnection::send(const char *data, size_t len) {
  send(StringSlice(data, len));
}

void TcpConnection::write(StringSlice msg) {
  if (!socket_)
    return;
  auto str = msg.toString();
  loop_->runInLoop([this, str]() {
    if (!socket_)
      return;
    assert(ioEvent_);
    assert(socket_);
    assert(loop_);
    if (!socket_->isConnected())
      return;
    if (ioEvent_->isWriting()) {
      writeBuffer_.write(str);
    } else {
      size_t written = 0;
      auto n = socket_->write(str.c_str(), str.size());
      if (n < 0 && perrno != PAGAIN) {
        LOG_ERROR("TcpConnection::write() write error");
        handleCloseEvent();
        return;
      }
      if (n > 0) {
        written += static_cast<size_t>(n);
      }
      if (written != str.size()) {
        writeBuffer_.write(str.c_str() + written, str.size() - written);
        ioEvent_->expectWriting();
      } else {
        if (writeCompleteCallback_)
          writeCompleteCallback_(this);
      }
    }
  });
}

void TcpConnection::write(const char *data, size_t len) {
  write(StringSlice(data, len));
}

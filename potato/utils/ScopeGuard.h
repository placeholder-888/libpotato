#ifndef POTATO_POTATO_UTILS_SCOPEGUARD_H_
#define POTATO_POTATO_UTILS_SCOPEGUARD_H_

#include "potato/net/SocketCommon.h"
#include "potato/utils/NonCopyable.h"
#include <functional>

namespace potato {

class SocketScopeGuard {
public:
  explicit SocketScopeGuard(socket_t socket) : socket_(socket) {}
  ~SocketScopeGuard() {
    if (socket_ != INVALID_SOCKET)
      potato::close(socket_);
  }
  void release() { socket_ = INVALID_SOCKET; }

private:
  socket_t socket_;
};

#define SocketScopeGuard(x) static_assert(false, "bad using")

class ScopeGuard : NonCopyable {
public:
  explicit ScopeGuard(std::function<void()> func) : func_(std::move(func)) {}
  ~ScopeGuard() { func_(); }

private:
  std::function<void()> func_;
};

#define ScopeGuard(x) static_assert(false, "bad using")

} // namespace potato

#endif // POTATO_POTATO_UTILS_SCOPEGUARD_H_

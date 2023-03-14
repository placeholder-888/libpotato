set(
    src
    net/IpAddress.cpp
    net/SocketCommon.cpp
    net/Socket.cpp
    net/EventLoop.cpp
    net/IOWatcher.cpp
    net/Acceptor.cpp
    net/Buffer.cpp
    net/TcpConnection.cpp
    net/TcpServer.cpp
    net/EventLoopPool.cpp
)

if (UNIX)
  link_libraries(pthread fmt)
else ()
  set(src ${src} ../3rdparty/wepoll/wepoll.c)
  link_libraries(ws2_32 wsock32 fmt)
endif ()
add_library(potato ${src})
include_directories(${PROJECT_SOURCE_DIR})
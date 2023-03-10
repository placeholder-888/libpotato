set(
    src
    net/IpAddress.cpp
    net/SocketCommon.cpp
    net/Socket.cpp
    net/EventLoop.cpp
    net/IOWatcher.cpp
    net/TcpSocket.cpp
    net/Acceptor.cpp
)

if (UNIX)
  link_libraries(pthread)
else ()
  set(src ${src} ${PROJECT_SOURCE_DIR}/wepoll/wepoll.c)
  link_libraries(ws2_32 wsock32)
endif ()
add_library(potato ${src})
include_directories(${PROJECT_SOURCE_DIR})
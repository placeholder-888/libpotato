set(
    src
    net/SocketCommon.cpp
)

if (UNIX)
  link_libraries(pthread)
else ()
  link_libraries(ws2_32 wsock32)
endif ()
add_library(potato ${src})

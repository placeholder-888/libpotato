include_directories(${PROJECT_SOURCE_DIR})

add_executable(EventLoopTest EventLoopTest.cpp)
target_link_libraries(EventLoopTest PRIVATE potato)

add_executable(IpAddressTest IpAddressTest.cpp)
target_link_libraries(IpAddressTest PRIVATE potato)

add_executable(SocketTestEchoServer SocketTestEchoServer.cpp)
target_link_libraries(SocketTestEchoServer PRIVATE potato)

add_executable(SocketTestEchoClient SocketTestEchoClient.cpp)
target_link_libraries(SocketTestEchoClient PRIVATE potato)

add_executable(PlainTcpServerTest PlainTcpServerTest.cpp)
target_link_libraries(PlainTcpServerTest PRIVATE potato)

add_executable(BufferTest BufferTest.cpp)
target_link_libraries(BufferTest PRIVATE potato)

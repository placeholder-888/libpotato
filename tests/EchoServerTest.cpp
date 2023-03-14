#include "potato/log/Logger.h"
#include "potato/net/TcpConnection.h"
#include "potato/net/TcpServer.h"

using namespace potato;

class EchoServer {
public:
  EchoServer(bool etMode) : server_(IpAddress(8888, true), etMode) {
    server_.setMessageCallback(
        [this](TcpConnection *conn) { onMessage(conn); });
    server_.setConnectionCallback(
        [this](TcpConnection *conn) { onConnection(conn); });
    server_.setWriteCompleteCallback(
        [this](TcpConnection *conn) { onWriteComplete(conn); });
  }

  void run() { server_.start(); }

private:
  void onConnection(TcpConnection *conn) {
    if (conn->isConnected()) {
      LOG_INFO("connection established [{}] : [{}]",
               conn->localAddress().IpPort(), conn->peerAddress().IpPort());
    } else {
      LOG_INFO("connection destroyed [{}] : [{}]",
               conn->localAddress().IpPort(), conn->peerAddress().IpPort());
    }
  }

  void onMessage(TcpConnection *conn) {
    StringSlice msg = conn->readBuffer().readAsSlice();
    LOG_INFO("received:{} from {}", msg.toString(),
             conn->peerAddress().IpPort());
    conn->send(msg);
    conn->readBuffer().retrieveAll();
  }

  void onWriteComplete(TcpConnection *conn) {
    LOG_INFO("write complete {}", conn->peerAddress().IpPort());
  }

  TcpServer server_;
};

int main(int argc, char **argv) {
  bool etMode = false;
  if (argc > 1)
    etMode = true;
  LOG_INFO("etMode:{}", etMode);
  EchoServer server(etMode);
  server.run();
}
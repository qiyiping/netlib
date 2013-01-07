#include <iostream>
#include <string>
#include "calc.hpp"
#include "simple_socket_server.hpp"
#include "event_socket_server.hpp"
#include "thread_pool_socket_server.hpp"
#include "epoll_socket_handler.hpp"

using namespace netlib;

void usage(const char *name) {
  // usage information
  std::cout << "Usage:" << std::endl
            << name << " server_type" << std::endl
            << "server_type: " << "simple/event/threadpool" << std::endl;
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    usage(argv[1]);
    return -1;
  }
  SocketServer *server;
  boost::shared_ptr<RequestHandler> handler(new CalcHandler());
  std::string host = "0.0.0.0";
  std::string port = "5555";
  if (strcmp(argv[1], "simple") == 0) {
    server = new SimpleSocketServer(host, port, handler);
  } else if (strcmp(argv[1], "event") == 0) {
    boost::shared_ptr<SocketEventHandler> eh(new EpollSocketEventHandler());
    boost::shared_ptr<EventLoop> el(new EventLoop(eh));
    server = new EventSocketServer(host, port, handler, el);
  } else if (strcmp(argv[1], "threadpool") == 0) {
    boost::shared_ptr<ThreadPool> tp(new ThreadPool(10, 100));
    server = new TPSocketServer(host, port, handler, tp);
  } else {
    usage(argv[1]);
    return -1;
  }
  server->Serve();
  return 0;
}

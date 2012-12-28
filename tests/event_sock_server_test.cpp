#include "event_socket_server.hpp"
#include <boost/make_shared.hpp>
#include <iostream>
#include "epoll_socket_handler.hpp"
#include "time.hpp"

using namespace netlib;

class MyHandler: public RequestHandler {
 public:
  void Process(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) {
    response->clear();
    response->append("echo from server: ");
    response->append(*request);
  }
};

void PrintTime(EventLoop *el, int32_t id) {
  std::cout << "current time: " << GetMilliSeconds() << std::endl
            << "------------------" << std::endl;
}

int main(int argc, char *argv[]) {
  EventSocketServer server("0.0.0.0", "10007",
                           boost::make_shared<MyHandler>(),
                           boost::make_shared<EventLoop>(boost::make_shared<EpollSocketEventHandler>()));
  server.GetEventLoop()->AddTimeEvent(0, 1000*5, PrintTime);
  server.Serve();
  return 0;
}

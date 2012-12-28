#include "simple_socket_server.hpp"
#include <boost/make_shared.hpp>
#include <string>
using namespace netlib;
class MyHandler: public RequestHandler {
 public:
  MyHandler(): RequestHandler(1000*10) {}
  void Process(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) {
    response->clear();
    response->append("echo from server: ");
    response->append(*request);
  }
};


int main(int argc, char *argv[]) {
  SimpleSocketServer server("0.0.0.0",
                            "10007",
                            boost::make_shared<MyHandler>());
  server.Serve();
  return 0;
}

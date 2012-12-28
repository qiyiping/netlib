#include "thread_pool_socket_server.hpp"
#include <boost/make_shared.hpp>
#include <string>
using namespace netlib;
class MyHandler: public RequestHandler {
 public:

  void Process(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) {
    response->clear();
    response->append("echo from server: ");
    response->append(*request);
  }
};


int main(int argc, char *argv[]) {
  TPSocketServer server("0.0.0.0",
                        "10007",
                        boost::make_shared<MyHandler>(),
                        boost::make_shared<ThreadPool>(10, 1000));
  server.Serve();
  return 0;
}

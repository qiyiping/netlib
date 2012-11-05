#include "simple_socket_server.hpp"
namespace netlib {
void SimpleSocketServer::Serve() {
  Listen();
  while (true) {
    int32_t fd = Accept();
    Handle(fd);
  }
}

void SimpleSocketServer::Handle(int32_t fd) {
  boost::shared_ptr<std::string> request(new std::string);
  boost::shared_ptr<std::string> response(new std::string);
  while (true) {
    if (request_handler_->SyncRecvRequest(fd, request) == RETURN_ERR) break;
    request_handler_->Process(request, response);
    if (request_handler_->SyncSendResponse(fd, response) == RETURN_ERR) break;
  }
  close(fd);
}

}

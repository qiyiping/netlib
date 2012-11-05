#include "thread_pool_socket_server.hpp"
#include <sys/socket.h>

namespace netlib {
void TPSocketServer::Serve() {
  Listen();
  while(true) {
    int32_t fd = Accept();
    if (fd >= 0) {
      thread_pool_->AddTask(std::tr1::bind(&TPSocketServer::Handle, this, fd));
    }
  }
}

void TPSocketServer::Handle(int32_t fd) {
  boost::shared_ptr<std::string> request(new std::string);
  boost::shared_ptr<std::string> response(new std::string);
  int32_t status;
  while (true) {
    status = request_handler_->SyncRecvRequest(fd, request);
    if (status == RETURN_ERR) break;
    request_handler_->Process(request, response);
    status = request_handler_->SyncSendResponse(fd, response);
    if (status == RETURN_ERR) break;
  }
  close(fd);
}

}

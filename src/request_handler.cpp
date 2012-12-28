#include "request_handler.hpp"
#include "socket_io.hpp"

namespace netlib {
// async methods
// AsyncRecvRequest recveive a request, processing the request with `Process'
// and register `AsyncSendResponse'
void RequestHandler::AsyncRecvRequest(EventLoop *el,
                                      int32_t fd,
                                      boost::shared_ptr<std::string> request) {
  request->clear();
  SocketIO io(fd);
  int32_t ret = io.ReadString(request.get());
  if (ret <= 0) {  // connection closed by the remote peer
    el->DeleteSocketEvent(fd);
    close(fd);
    return;
  }
  boost::shared_ptr<std::string> response(new std::string);
  Process(request, response);
  SocketCallback cb = std::tr1::bind(&RequestHandler::AsyncSendResponse,
                                     this,
                                     std::tr1::placeholders::_1,
                                     std::tr1::placeholders::_2,
                                     response);
  el->ModifySocketWriteEvent(fd, cb);
}

void RequestHandler::AsyncSendResponse(EventLoop *el,
                                       int32_t fd,
                                       boost::shared_ptr<std::string> response) {
  SocketIO io(fd);
  int32_t ret = io.WriteString(*response);
  if (ret <= 0) { // connection is reset
    el->DeleteSocketEvent(fd);
    close(fd);
    return;
  }
  el->ModifySocketWriteEvent(fd, NULL);
}

// sync methods
int32_t RequestHandler::SyncRecvRequest(int32_t fd,
                                        boost::shared_ptr<std::string> request) {
  request->clear();
  SocketIO io(fd, timeout_, timeout_);
  if (io.ReadString(request.get()) > 0) return RETURN_OK;
  return RETURN_ERR;
}

int32_t RequestHandler::SyncSendResponse(int32_t fd,
                                         boost::shared_ptr<std::string> response) {
  SocketIO io(fd, timeout_, timeout_);
  if (io.WriteString(*response) > 0) return RETURN_OK;
  return RETURN_ERR;
}

}

#include "socket_client.hpp"
#include "socket_io.hpp"
#include <iostream>
#include <time.h>
#include <glog/logging.h>

int main(int argc, char *argv[]) {
  if (argc < 3)
    return -1;
  netlib::SocketClient client(argv[1], argv[2], false);
  netlib::SocketIO io(client.GetSocket());

  std::string response, request;
  int32_t status;
  while(true) {
    std::getline(std::cin, request);
    if (request.empty()) continue;
    status = io.WriteString(request);
    if (status <= 0) break;
    status = io.ReadString(&response);
    if (status <= 0 ) break;
    std::cout << response << std::endl;
  }

  return 0;
}

#include "event_loop.hpp"
#include "epoll_socket_handler.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <glog/logging.h>
#include <string>
#include <iostream>
#include <boost/make_shared.hpp>
using namespace std;
using namespace netlib;

void ReadProc(EventLoop *el, int32_t fd) {
  char buf[1024];
  int32_t len = recv(fd, buf, sizeof(buf), 0);
  if (len < 0) {
    LOG(ERROR) << "error in reading fd: " << fd;
  } else if (len == 0) {
    el->DeleteSocketEvent(fd);
    close(fd);
  } else {
    string s(buf, buf+len);
    cout << "received from " << fd << ":" << s << endl;
    send(fd, s.c_str(), s.length(), 0);
    if (s.find("kill") != string::npos) {
      el->SetStop();
    }
    if (s.find("quit") != string::npos) {
      el->DeleteSocketEvent(fd);
      close(fd);
    }
  }
}



void WriteProc(EventLoop *el, int32_t fd) {
  std::string welcome = "Hello! Welcome\n";
  int32_t len = send(fd, welcome.c_str(), welcome.length(), 0);
  if (len < 0) {
    LOG(ERROR) << "sending error";
    el->DeleteSocketEvent(fd);
    close(fd);
  }
  el->ModifySocketWriteEvent(fd, NULL);
}

void AcceptProc(EventLoop *el, int32_t fd) {
  struct sockaddr_storage ss;
  socklen_t slen = sizeof(ss);
  int32_t conn_sock = accept(fd, (struct sockaddr *)&ss, &slen);
  CHECK_NE(conn_sock, -1) << "error in accept";
  fcntl(conn_sock, F_SETFL, O_NONBLOCK);
  el->AddSocketEvent(conn_sock, EVENT_READ|EVENT_WRITE, ReadProc, WriteProc);
}

void TimeProc1(EventLoop *el, int32_t id) {
  cout << "*************" << endl;
  cout << "now: " << GetMilliSeconds() << endl;
  cout << "*************" << endl;
}

void TimeProc2(EventLoop *el, int32_t id) {
  cout << "*************" << endl;
  cout << "take a rest" << endl;
  cout << "*************" << endl;
}

int main(int argc, char *argv[]) {
  EventLoop el(boost::make_shared<EpollSocketEventHandler>());
  // set up the listener
  int32_t listener = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in sin;
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = 0;
  sin.sin_port = htons(10007);
  bind(listener, (struct sockaddr *)&sin, sizeof(sin));
  fcntl(listener, F_SETFL, O_NONBLOCK);
  listen(listener, 16);
  el.AddSocketEvent(listener, EVENT_READ, AcceptProc, NULL);
  int64_t now = GetMilliSeconds();
  el.AddTimeEvent(now-1000, 30*1000, TimeProc1);
  el.AddTimeEvent(now-2000, -1, TimeProc2);
  el.Main();
  return 0;
}


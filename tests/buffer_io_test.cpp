#include "buffer_io.hpp"
#include <glog/logging.h>

using namespace netlib;

int main(int argc, char *argv[]) {
  BufferIO io;
  io.WriteInt32(1021);
  io.WriteDouble(3.14159);
  io.WriteString("buffer io test");
  LOG(INFO) << io.GetBuffer();
  int32_t a = 0;
  io.ReadInt32(&a);
  LOG(INFO) << a;
  double b = 0;
  io.ReadDouble(&b);
  LOG(INFO) << b;
  std::string str;
  io.ReadString(&str);
  LOG(INFO) << str;
  return 0;
}

#include "file_io.hpp"
#include <glog/logging.h>
using namespace netlib;
int main(int argc, char *argv[]) {
  File h("a.bin", OPEN_WRITE);
  CHECK_EQ(h.WriteInt64(98765), RETURN_OK);
  CHECK_EQ(h.WriteInt32(10247), RETURN_OK);
  CHECK_EQ(h.WriteDouble(3.1415926), RETURN_OK);
  std::string str1("file io test");
  CHECK_EQ(h.WriteString(str1), str1.length());
  h.Close();
  CHECK_EQ(h.Open("a.bin", OPEN_READ), RETURN_OK);
  int64_t a;
  int32_t b;
  double pi;
  std::string str2;
  CHECK_EQ(h.ReadInt64(&a), RETURN_OK);
  h.ReadInt32(&b);
  h.ReadDouble(&pi);
  h.ReadString(&str2);
  LOG(INFO) << a << ", " << pi << ", " << b << ", " << str2;
  return 0;
}

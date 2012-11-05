#include "bloom_filter.hpp"
#include <glog/logging.h>
using namespace netlib;
int main(int argc, char *argv[]) {
  BloomFilter filter(10000000, 0.01);
  filter.Insert("www.sina.com.cn");
  filter.Insert("www.163.com");
  LOG(INFO) << "www.qq.com: " << filter.Exists("www.qq.com");
  LOG(INFO) << "www.163.com: " << filter.Exists("www.163.com");
  filter.WriteToFile("filter.bin");
  BloomFilter ff("filter.bin");
  LOG(INFO) << "www.qq.com: " << ff.Exists("www.qq.com");
  LOG(INFO) << "www.163.com: " << ff.Exists("www.163.com");
  return 0;
}

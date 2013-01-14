#include "calc.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  std::string host = "0.0.0.0";
  std::string port = "5555";
  if (argc > 2) {
    host = argv[1];
    port = argv[2];
  }
  CalcClient client(host, port);
  double a, b;
  while (true) {
    std::cin >> a >> b;
    std::cout << a << "+" << b << " = " << client.Add(a,b) << std::endl;
    std::cout << a << "-" << b << " = " << client.Sub(a,b) << std::endl;
    std::cout << "sum(" << a << "," << b << ") = " << client.Sum(a,b) << std::endl;
  }
  return 0;
}

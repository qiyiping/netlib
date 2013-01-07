#ifndef _CALC_H_
#define _CALC_H_

//
// Simple calculator example
//
#include "dispatch_handler.hpp"
#include "buffer_io.hpp"
#include "socket_io.hpp"

using namespace netlib;

class CalcHandler: public DispatchHandler {
 public:
  CalcHandler(int32_t timeout = -1): DispatchHandler(timeout) {
    AddProcessor("add", std::tr1::bind(&CalcHandler::Add, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    AddProcessor("sub", std::tr1::bind(&CalcHandler::Sub, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  }
  void Add(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) {
    BufferIO in(*request);
    uint8_t id_len = 0;
    if (in.ReadUInt8(&id_len) != 1)
      return;
    if (in.SkipBytes(id_len) != id_len)
      return;
    double a = 0;
    double b = 0;
    if (in.ReadDouble(&a) != sizeof(double))
      return;
    if (in.ReadDouble(&b) != sizeof(double))
      return;
    BufferIO out;
    out.WriteDouble(a+b);
    *response = out.GetBuffer();
  }

  void Sub(boost::shared_ptr<std::string> request, boost::shared_ptr<std::string> response) {
    BufferIO in(*request);
    uint8_t id_len = 0;
    if (in.ReadUInt8(&id_len) != 1)
      return;
    if (in.SkipBytes(id_len) != id_len)
      return;
    double a = 0;
    double b = 0;
    if (in.ReadDouble(&a) != sizeof(double))
      return;
    if (in.ReadDouble(&b) != sizeof(double))
      return;
    BufferIO out;
    out.WriteDouble(a-b);
    *response = out.GetBuffer();
  }
};

class CalcClient {
 public:
  CalcClient(const std::string &host, const std::string &port): socket_(host, port), io_(socket_) {}
  double Add(double a, double b) {
    std::string id = BuildHeader("add");
    BufferIO buf(id);
    buf.WriteDouble(a);
    buf.WriteDouble(b);
    io_.WriteString(buf.GetBuffer());
    double ret = 0;
    io_.ReadDouble(&ret);
    return ret;
  }

  double Sub(double a, double b) {
    std::string id = BuildHeader("sub");
    BufferIO buf(id);
    buf.WriteDouble(a);
    buf.WriteDouble(b);
    io_.WriteString(buf.GetBuffer());
    double ret = 0;
    io_.ReadDouble(&ret);
    return ret;
  }

  virtual ~CalcClient() {}
 private:
  SocketClient socket_;
  SocketIO io_;
};

#endif /* _CALC_H_ */

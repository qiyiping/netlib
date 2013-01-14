#ifndef _CALC_H_
#define _CALC_H_

//
// Simple calculator example
//
#include "dispatch_handler.hpp"
#include "buffer_io.hpp"
#include "socket_io.hpp"
#include "serialize.hpp"
#include <glog/logging.h>

using namespace netlib;

class CalcRequest: public Serializable {
 public:
  bool Serialize(IOBase *io) {
    io->WriteDouble(arg1);
    io->WriteDouble(arg2);
    return true;
  }
  bool Deserialize(IOBase *io) {
    io->ReadDouble(&arg1);
    io->ReadDouble(&arg2);
    return true;
  }
  double arg1;
  double arg2;
};

class CalcResponse: public Serializable {
 public:
  bool Serialize(IOBase *io) {
    io->WriteDouble(result);
    return true;
  }
  bool Deserialize(IOBase *io) {
    io->ReadDouble(&result);
    return true;
  }
  double result;
};

class CalcHandler: public DispatchHandler {
 public:
  CalcHandler(int32_t timeout = -1): DispatchHandler(timeout) {
    AddProcessor("add", std::tr1::bind(&CalcHandler::Add, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    AddProcessor("sub", std::tr1::bind(&CalcHandler::Sub, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
    AddProcessor("sum", std::tr1::bind(&CalcHandler::Sum, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2));
  }
  void Add(const std::string &request, std::string *response) {
    BufferIO in(request);
    CalcRequest req;
    req.Deserialize(&in);
    CalcResponse resp;
    resp.result = req.arg1 + req.arg2;
    BufferIO out;
    resp.Serialize(&out);
    *response = out.GetBuffer();
  }

  void Sub(const std::string &request, std::string *response) {
    BufferIO in(request);
    CalcRequest req;
    req.Deserialize(&in);
    CalcResponse resp;
    resp.result = req.arg1 - req.arg2;
    BufferIO out;
    resp.Serialize(&out);
    *response = out.GetBuffer();
  }

  void Sum(const std::string &request, std::string *response) {
    BufferIO in(request);
    CalcRequest req;
    req.Deserialize(&in);

    CalcResponse resp;
    resp.result = 0.0;
    for (; req.arg1 < req.arg2; req.arg1 += 1.0) {
      resp.result += req.arg1;
    }

    BufferIO out;
    resp.Serialize(&out);
    *response = out.GetBuffer();
  }
};

class CalcClient {
 public:
  CalcClient(const std::string &host, const std::string &port): socket_(host, port), io_(socket_) { CHECK(socket_.IsConnected()); }
  double Add(double a, double b) {
    BufferIO buf(BuildHeader("add"));
    CalcRequest req;
    req.arg1 = a;
    req.arg2 = b;
    req.Serialize(&buf);
    io_.WriteString(buf.GetBuffer());
    CalcResponse resp;
    resp.Deserialize(&io_);
    return resp.result;
  }

  double Sub(double a, double b) {
    BufferIO buf(BuildHeader("sub"));
    CalcRequest req;
    req.arg1 = a;
    req.arg2 = b;
    req.Serialize(&buf);
    io_.WriteString(buf.GetBuffer());
    CalcResponse resp;
    resp.Deserialize(&io_);
    return resp.result;
  }

  double Sum(double a, double b) {
    BufferIO buf(BuildHeader("sum"));
    CalcRequest req;
    req.arg1 = a;
    req.arg2 = b;
    req.Serialize(&buf);
    io_.WriteString(buf.GetBuffer());
    CalcResponse resp;
    resp.Deserialize(&io_);
    return resp.result;
  }

  virtual ~CalcClient() {}
 private:
  SocketClient socket_;
  SocketIO io_;
};

#endif /* _CALC_H_ */

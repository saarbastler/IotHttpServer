#ifndef _INCLUDE_SERIAL_HOST_H_
#define _INCLUDE_SERIAL_HOST_H_

#include <memory>
#include <thread>

#include <boost/asio/deadline_timer.hpp>

#include "PlcSerial.h"
#include "PlcModel.h"

class SerialHost
{
public:
  static constexpr unsigned NUMBER_OF_INPUTS =  8;

  SerialHost(saba::plc::PlcModel& plcModel, boost::asio::io_service& ios) : plcModel(plcModel), serial(ios, 80) {}

  ~SerialHost()
  {
    close();
  }

  void open(const char *comPort, unsigned baudrate);
  void reopen();

  void close();

private:

  void eventListener(Event& event);  

  PlcSerial serial;
  saba::plc::PlcModel& plcModel;

  std::string port;
  unsigned baudrate;
};

#endif // _INCLUDE_SERIAL_HOST_H_


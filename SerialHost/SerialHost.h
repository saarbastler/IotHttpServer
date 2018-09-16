#ifndef _INCLUDE_SERIAL_HOST_H_
#define _INCLUDE_SERIAL_HOST_H_

#include <memory>
#include <thread>
#include <sstream>
#include <iostream>

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

  void monoflop(unsigned channel, bool on)
  {
    std::ostringstream out;
    out << 'M' << channel << (on ? 'S' : 'R') << "\r\n";

    std::cout << "<|" << out.str();
    serial.send(out.str().c_str(), int(out.str().length()));
  }

  void monoflopTime(unsigned channel, uint16_t time)
  {
    std::ostringstream out;
    out << 'M' << channel << 'T' << time << "\r\n";

    std::cout << "<|" << out.str();
    serial.send(out.str().c_str(), int(out.str().length()));
  }

private:

  void eventListener(Event& event);  

  PlcSerial serial;
  saba::plc::PlcModel& plcModel;

  std::string port;
  unsigned baudrate;
};

#endif // _INCLUDE_SERIAL_HOST_H_


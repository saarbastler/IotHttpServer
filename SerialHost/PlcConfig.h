#ifndef _INCLUDE_PLC_CONFIG_H_
#define _INCLUDE_PLC_CONFIG_H_

#include <config.h>

namespace saba
{
  constexpr const char *SERIAL_PORT = "config.serial.port";
  constexpr const char *SERIAL_BAUDRATE = "config.serial.baudrate";

  class PlcConfig : public web::Config
  {
  public:

    const std::string& getSerialPort()
    {
      return port;
    }

    const unsigned getSerialBaudrate()
    {
      return baudrate;
    }

  protected:

    virtual void readMore(boost::property_tree::ptree& tree) 
    {
      port = tree.get<std::string>(SERIAL_PORT);
      baudrate = tree.get<unsigned>(SERIAL_BAUDRATE);
    }

    std::string port;
    unsigned baudrate;

  };
}

#endif // !_INCLUDE_PLC_CONFIG_H_


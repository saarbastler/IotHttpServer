#ifndef _INCLUDE_PLC_CONFIG_H_
#define _INCLUDE_PLC_CONFIG_H_

#include <config.h>

namespace saba
{
  constexpr const char *SERIAL_PORT = "config.serial.port";
  constexpr const char *SERIAL_BAUDRATE = "config.serial.baudrate";
  constexpr const char *UPLOAD_DIR = "config.uploaddir";
  constexpr const char *AVRDUDE = "config.avrdude";

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

    const std::string& getUploadDir()
    {
      return uploaddir;
    }

    const std::string& getAvrdude()
    {
      return avrdude;
    }

  protected:

    virtual void readMore(boost::property_tree::ptree& tree) 
    {
      port = tree.get<std::string>(SERIAL_PORT);
      baudrate = tree.get<unsigned>(SERIAL_BAUDRATE);
      uploaddir = tree.get<std::string>(UPLOAD_DIR);
      avrdude = tree.get<std::string>(AVRDUDE);
    }

    std::string port;
    unsigned baudrate;
    std::string uploaddir;
    std::string avrdude;
  };
}

#endif // !_INCLUDE_PLC_CONFIG_H_


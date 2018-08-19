#include <iostream>
#include <string>

#include <boost/filesystem.hpp>

#include <server.hpp>

#include <FileServer.h>

#include "PlcConfig.h"
#include "SerialHost.h"
#include "PlcRestController.h"

int main(int argc, char* argv[])
{
  if (argc != 2)
  {
    std::cout << "Usage:" << std::endl
      << "IotHttpServer config.xml" << std::endl;

    exit(-1);
  }

  const std::string file = argv[1];
  saba::PlcConfig config;

  http::server my_http_server;

  try
  {
    saba::plc::PlcModel plcModel;
    SerialHost serialHost(plcModel,http::base::processor::get().io_service());
    config.read(file, [&my_http_server, &config](const std::string& uri, const std::string& path, const std::string& defaultFile)
    {
      auto fileServer = std::make_shared<saba::web::FileServer>(path, defaultFile);

      my_http_server.get(uri, [fileServer, &config](auto& req, auto& session, auto& arguments)
      {
        fileServer->serve(req, session, config);
      });
    });
    
    serialHost.open(config.getSerialPort().c_str(), config.getSerialBaudrate());

    PlcRestController plcRestController(my_http_server, plcModel);

    my_http_server.listen(config.getAddress(), boost::lexical_cast<uint32_t>(config.getPort()), [](auto & session) {
      session.do_read();
    });

    //##################################################################

    std::cout << "Server starting on " << config.getAddress() << ':' << config.getPort() << std::endl;

    //##################################################################

    http::base::processor::get().register_signals_handler([](int) 
    {
      std::cout << "\nPlease wait!" << std::endl;
      http::base::processor::get().stop();
    }, std::vector<int>{SIGINT, SIGTERM/*, SIGQUIT*/});

    uint32_t pool_size = boost::lexical_cast<uint32_t>(config.getThreads());
    http::base::processor::get().start(pool_size == 0 ? 4 : pool_size << 1);
    http::base::processor::get().wait();
  }
  catch (std::exception& e)
  {
    std::cout << e.what() << std::endl;
  }
}

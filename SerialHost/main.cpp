#include <iostream>
#include <fstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <server.hpp>

#include <FileServer.h>
#include <MultipartParser.h>

#include "PlcConfig.h"
#include "SerialHost.h"
#include "PlcRestController.h"
#include "Process.h"

#include <boost/asio.hpp>

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
    SerialHost serialHost(plcModel, http::base::processor::get().io_service());
    config.read(file, [&my_http_server, &config](const std::string& uri, const std::string& path, const std::string& defaultFile)
    {
      auto fileServer = std::make_shared<saba::web::FileServer>(path, defaultFile);

      my_http_server.get(uri, [fileServer, &config](auto& req, auto& session)
      {
        fileServer->serve(req, session, config);
      });
    });
        

    Process proc(http::base::processor::get().io_service());

    serialHost.open(config.getSerialPort().c_str(), config.getSerialBaudrate());

    PlcRestController plcRestController(http::base::processor::get().io_service(), my_http_server, plcModel, config, serialHost);

    my_http_server.options(".*", [](auto& req, auto& session)
    {
      boost::beast::http::response<boost::beast::http::string_body> res{ boost::beast::http::status::ok, req.version() };

      res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set("Access-Control-Allow-Origin", "*");
      res.set("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE");
      res.keep_alive(req.keep_alive());
      res.prepare_payload();

      session.do_write(std::move(res));
    });
    my_http_server.all(".*", [](auto& req, auto& session)
    {
      session.do_write(std::move(saba::web::errorResponse(req,boost::beast::http::status::not_found, "Resource not found")));
    });

    my_http_server.listen(config.getAddress(), boost::lexical_cast<uint32_t>(config.getPort()), [](auto & session) {
      session.do_read();
    });

    plcModel.addListObserver([&my_http_server](saba::plc::DataType dataType, unsigned index, bool value)
    {
      std::ostringstream out;
      out << "{ \"type\":\"" << saba::plc::DataTypeNames[int(dataType)] << "\",\"index\":" << index << ",\"value\":" << (value ? "true" : "false") << "}";

      my_http_server.websockets()->sendToAll(out.str());
    });

    /*plcModel.get(saba::plc::DataType::Outputs, 0).addObserver([&my_http_server](const bool& value)
    {
      std::ostringstream out;
      out << "{ \"type\":\"output\",\"index\":" << 0 << ",\"value\":" << (value ? "true" : "false") << "}";

      my_http_server.websockets()->sendToAll(out.str());
    });*/

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


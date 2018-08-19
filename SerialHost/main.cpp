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
    
    my_http_server.post("/upload", [&config](auto& req, auto& session, auto& arguments)
    {
      boost::string_view content = req.body();

      try
      {
        saba::web::MultipartParser multipartParser(req.body());

        std::cout << "/upload: " << multipartParser.getFilename() << std::endl;
        std::string now= boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time())
          + '_' + multipartParser.getFilename();

        boost::filesystem::path path(config.getUploadDir());
        path /= now;

        std::ofstream out(path.c_str(), std::ofstream::out | std::ofstream::trunc);
        if (!out.is_open() || out.bad())
        {
          std::string utf_path(boost::locale::conv::utf_to_utf<char>(path.c_str()));

          throw saba::Exception("Unable to open file for write: %s", utf_path.c_str());
        }

        out.write(multipartParser.getFileStart(), multipartParser.getFilesize());
        out.close();

        session.do_write(std::move(saba::web::errorResponse(req, "", boost::beast::http::status::ok)));
      }
      catch (std::exception& ex)
      {
        session.do_write(std::move(saba::web::errorResponse(req, ex)));
      }
    });


    serialHost.open(config.getSerialPort().c_str(), config.getSerialBaudrate());

    PlcRestController plcRestController(my_http_server, plcModel, config);

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


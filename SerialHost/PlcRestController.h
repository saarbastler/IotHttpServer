#ifndef _INCLUDE_PLC_REST_CONTROLLER_H_
#define _INCLUDE_PLC_REST_CONTROLLER_H_

#include <sstream>
#include <server.hpp>
#include <thread>
#include <chrono>

#include <boost/filesystem.hpp>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

#include "PlcModel.h"
#include "PlcConfig.h"
#include "Comma.h"
#include "Response.h"
#include "Process.h"

// gcc does not find this constant, when this is class constant
static constexpr const char *Filemacro = "${FILE}";

class PlcRestController
{
public:

  PlcRestController(boost::asio::io_service& ios, http::server& http_server, saba::plc::PlcModel& plcModel
    , saba::PlcConfig& config, SerialHost& serialHost)
    : plcModel(plcModel), config(config), avrdude(ios), serialHost(serialHost)
  {
    http_server.param<std::string>().get("/api/(inputs|outputs|merker|monoflops)",[this](auto& req, auto& session, auto& args)
    {
      using namespace saba::plc;
      DataType dataType = DataType::Inputs;

      try
      {
        /*if (arguments.empty())
          throw PlcException("missing arguments");
        else*/ if (args._1 == "outputs")
          dataType = DataType::Outputs;
        else if (args._1 == "merker")
          dataType = DataType::Merker;
        else if (args._1 == "monoflops")
          dataType = DataType::Monoflops;
        else if (args._1 != "inputs")
          throw PlcException("Not found: %s", args._1.c_str());

        std::string json = this->prepareArrayResponse(dataType);

        session.do_write(std::move(saba::web::jsonResponse(req, json)));
      }
      catch (std::exception& ex)
      {
        session.do_write(std::move(saba::web::errorResponse(req, ex)));
      }
    });

    http_server.get("/api/uploads", [this](auto& req, auto& session)
    {
      using namespace boost::filesystem;

      try
      {
        directory_iterator it(path(this->config.getUploadDir()));

        std::ostringstream out;

        out << '[';
        Comma comma;
        for (auto& entry : boost::make_iterator_range(it, {}))
          out << comma << entry.path().filename();
          out << ']';

          session.do_write(std::move(saba::web::jsonResponse(req, out.str())));
      }
      catch (std::exception& ex)
      {
        session.do_write(std::move(saba::web::errorResponse(req, ex)));
      }
    });

    http_server.post("/api/upload", [&config](auto& req, auto& session)
    {
      boost::string_view content = req.body();

      try
      {
        saba::web::MultipartParser multipartParser(req.body());

        std::string now = boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time())
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

    http_server.param<std::string>().get("/api/flash/(.*)", [this](auto& req, auto& session, auto& args)
    {
      using namespace boost::filesystem;

      try
      {
        if (this->avrdude.isRunning())
          throw PlcException("AvrDude Process is still running.");

        //if (arguments.size() != 1)
        //  throw PlcException("missing arguments");

        path path(this->config.getUploadDir());
        //path /= arguments[0];
        path /= args._1;
        if(!is_regular_file(status(path)))
          throw PlcException("File does not exist: %s", args._1.c_str());

        this->serialHost.close();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        std::string commandline = this->config.getAvrdude();
        std::string utf_path(boost::locale::conv::utf_to_utf<char>(path.c_str()));
        auto it = commandline.find(Filemacro);
        if (it != std::string::npos)
          boost::algorithm::replace_all(commandline, Filemacro, utf_path);

        // serialHost or this captured is not working, the captured value is invalid
        this->avrdude.startProcess(session.getConnection(), commandline, [](void *arg)
        {
          SerialHost *pserialHost = static_cast<SerialHost*>(arg);
          pserialHost->reopen();
        }, &this->serialHost);

      }
      catch (std::exception& ex)
      {
        session.do_write(std::move(saba::web::errorResponse(req, ex)));
      }
    });
  }

protected:

  std::string prepareArrayResponse(saba::plc::DataType dataType)
  {
    auto list = plcModel.getList(dataType);
    std::ostringstream out;

    out << '[';
    Comma comma;
    for (auto it = list.begin(); it != list.end(); it++)
      out << comma << (it->get() ? '1' : '0');
    out << ']';

    return std::move(out.str());
  }

private:

  saba::plc::PlcModel& plcModel;
  saba::PlcConfig& config;
  Process avrdude;
  SerialHost& serialHost;
};

#endif // !_INCLUDE_PLC_REST_CONTROLLER_H_


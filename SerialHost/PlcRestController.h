#ifndef _INCLUDE_PLC_REST_CONTROLLER_H_
#define _INCLUDE_PLC_REST_CONTROLLER_H_

#include <sstream>
#include <server.hpp>

#include "PlcModel.h"
#include "Comma.h"
#include "Response.h"


class PlcRestController
{
public:

  PlcRestController(http::server& http_server, saba::plc::PlcModel& plcModel) : plcModel(plcModel)
  {
    http_server.get("/api/(inputs|outputs|merker|monoflops)",[this](auto& req, auto& session, auto& arguments)
    {
      using namespace saba::plc;
      DataType dataType = DataType::Inputs;

      try
      {
        if (arguments.empty())
          throw PlcException("missing arguments");
        else if (arguments[0] == "outputs")
          dataType = DataType::Outputs;
        else if (arguments[0] == "merker")
          dataType = DataType::Merker;
        else if (arguments[0] == "monoflops")
          dataType = DataType::Monoflops;
        else if (arguments[0] != "inputs")
          throw PlcException("Not found: %s", arguments[0].c_str());

        std::string json = this->prepareArrayResponse(dataType);

        session.do_write(std::move(saba::web::jsonResponse(req, json)));
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
};

#endif // !_INCLUDE_PLC_REST_CONTROLLER_H_


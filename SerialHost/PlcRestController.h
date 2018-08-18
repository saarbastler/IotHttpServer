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
    http_server.get("/api/(inputs|outputs)",[this](auto& req, auto& session, auto& arguments)
    {
      using namespace saba::plc;

      DataType dataType = DataType::Inputs;
      if (!arguments.empty() && arguments[0] == "outputs")
        dataType = DataType::Outputs;

      std::string json = prepareArrayResponse(dataType);

      session.do_write(std::move(saba::web::jsonResponse(req, json)));
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


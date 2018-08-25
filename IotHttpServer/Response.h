#ifndef _INCLUDE_RESPONSE_H_
#define _INCLUDE_RESPONSE_H_

#include <exception>
#include <string>
#include <cstdarg>
#include <sstream>

#include <boost/beast.hpp>

namespace saba
{
  namespace web
  {

    template<class Request>
    auto jsonResponse(Request& req, const std::string& jsonText, boost::beast::http::status status = boost::beast::http::status::ok)
    {
      boost::beast::http::response<boost::beast::http::string_body> res{ status, req.version() };

      res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(boost::beast::http::field::content_type, "application/json");
      res.set("Access-Control-Allow-Origin", "*");
      res.keep_alive(req.keep_alive());
      res.body() = jsonText;
      res.prepare_payload();

      return res;
    }


    template<class Request>
    auto errorResponse(Request& req, const std::string& text, boost::beast::http::status status = boost::beast::http::status::internal_server_error)
    {
      boost::beast::http::response<boost::beast::http::string_body> res{ status, req.version() };

      res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
      res.set(boost::beast::http::field::content_type, "text/html");
      res.keep_alive(req.keep_alive());
      res.body() = text;
      res.prepare_payload();

      return res;
    }

    template<class Request>
    auto errorResponse(Request& req, const std::exception& ex, boost::beast::http::status status = boost::beast::http::status::bad_request)
    {
      return errorResponse(req, ex.what(), status);
    }

    template<class Request>
    auto errorResponse(Request& req, boost::beast::http::status status, const char *format, ...)
    {
      va_list args;
      va_start(args, format);

      char buffer[1000];
      vsnprintf(buffer, 1000, format, args);
      va_end(args);

      std::string message(buffer);
      return errorResponse(req, message, status);
    }

    template<class Request>
    auto errorResponseNotFound(Request& req)
    {
      std::ostringstream out;
      out << "Resource " << req.target().to_string() << " not found.";

      return errorResponse(req, out.str(), boost::beast::http::status::not_found);
    }
  }
}

#endif // !_INCLUDE_RESPONSE_H_


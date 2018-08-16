#ifndef _INCLUDE_FILE_SERVER_H_
#define _INCLUDE_FILE_SERVER_H_

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

#include <server.hpp>

#include "UriParser.h"

// Returns a bad request response
template<class Request>
auto bad_request(Request & req, const std::string & why)
{
  boost::beast::http::response<boost::beast::http::string_body> res
  {
    boost::beast::http::status::bad_request,
    req.version() };

  res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(boost::beast::http::field::content_type, "text/html");
  res.keep_alive(req.keep_alive());
  res.body() = why;
  res.prepare_payload();
  return res;
};

// Returns a not found response
template<class Request>
auto not_found(Request & req)
{
  boost::beast::http::response<boost::beast::http::string_body> res
  {
      boost::beast::http::status::not_found, req.version() };

  res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(boost::beast::http::field::content_type, "text/html");
  res.keep_alive(req.keep_alive());
  res.body() = "The resource '" + req.target().to_string() + "' was not found.\n";
  res.prepare_payload();
  return res;
};

// Returns a server error response
template<class Request>
auto server_error(Request & req, const boost::beast::string_view & what)
{
  boost::beast::http::response<boost::beast::http::string_body> res
  {
      boost::beast::http::status::internal_server_error, req.version() };

  res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
  res.set(boost::beast::http::field::content_type, "text/html");
  res.keep_alive(req.keep_alive());
  res.body() = "An error occurred: '" + what.to_string() + "'\n";
  res.prepare_payload();
  return res;
};

namespace saba
{
  namespace web
  {
    class FileServer
    {
    private:

      std::string ospath;
      std::string defaultFile;

    public:
      FileServer(std::string& path, std::string& defaultFile)
        : ospath(std::move(path)), defaultFile(std::move(defaultFile))
      { }

      ~FileServer()
      {

      }

      template<class Request, class Session, class Mimetype>
      void serve(Request& req, Session& session, Mimetype& mimetype)
      {
        std::cout << req.method_string() << ":" << req.target() << " ";

        try
        {
          auto target = req.target();
          saba::web::UriParser uri(target);

          std::string relpath = uri.getPath();
          if (relpath.empty() || relpath == "/")
            relpath = defaultFile;

          boost::filesystem::path path(ospath);
          path /= relpath;

          std::string utf_path(boost::locale::conv::utf_to_utf<char>(path.c_str()));

          boost::beast::error_code ec;
          boost::beast::http::file_body::value_type body;
          body.open(utf_path.c_str(), boost::beast::file_mode::scan, ec);

          if (ec == boost::system::errc::no_such_file_or_directory)
          {
            std::cout << "not found: " << utf_path << std::endl;

            session.do_write(not_found(req));
          }
          // Handle an unknown error
          else if (ec)
          {
            std::cout << "error opening " << utf_path << " : " << ec << std::endl;

            session.do_write(server_error(req, ec.message()));
          }
          else
          {
            auto const body_size = body.size();

            std::cout << "serving " << utf_path << " size: " << body_size << std::endl;

            boost::beast::http::response<boost::beast::http::file_body> res{
                std::piecewise_construct,
                std::make_tuple(std::move(body)),
                std::make_tuple(boost::beast::http::status::ok, req.version()) };

            res.set(boost::beast::http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(boost::beast::http::field::content_type, mimetype[path.extension()]);
            res.content_length(body_size);
            res.keep_alive(req.keep_alive());

            session.do_write(std::move(res));
          }
        }
        catch (std::exception& ex)
        {
          session.do_write(bad_request(req, ex.what()));
        }
      }
    };

  }
}

#endif // !_INCLUDE_FILE_SERVER_H_



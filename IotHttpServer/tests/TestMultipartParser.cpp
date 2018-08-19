#ifdef IOT_HTTP_SERVER_TESTS

#include <iostream>

#include "../MultipartParser.h"

#define BOOST_TEST_MODULE MultibyteParserTest
#include <boost/test/unit_test.hpp>

using namespace saba::web;

/*int main(int argc, char* argv[])
{
  std::string body("-----------------------------23191515113041\r\nContent-Disposition: form-data; name=\"file\"; filename=\"Aussenbeleuchtung.eep\"\r\nContent-Type: application/octet-stream\r\n\r\n:00000001FF\r\n\r\n-----------------------------23191515113041--\r\n");
  MultipartParser multipartParser(body);

  boost::string_view content(multipartParser.getFileStart(), multipartParser.getFilesize());
  bool b = content == ":00000001FF\r\n";
}*/

BOOST_AUTO_TEST_CASE(MultipartParser_parse1)
{
  std::string body("-----------------------------23191515113041\r\nContent-Disposition: form-data; name=\"file\"; filename=\"Aussenbeleuchtung.eep\"\r\nContent-Type: application/octet-stream\r\n\r\n:00000001FF\r\n\r\n-----------------------------23191515113041--\r\n");
  MultipartParser multipartParser(body);

  BOOST_CHECK_EQUAL(multipartParser.getFilename(), "Aussenbeleuchtung.eep");
  BOOST_CHECK_EQUAL(multipartParser.getContentType(), "application/octet-stream");

  boost::string_view content(multipartParser.getFileStart(), multipartParser.getFilesize());
  BOOST_CHECK(content == ":00000001FF\r\n");
}

#endif



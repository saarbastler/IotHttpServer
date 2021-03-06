#ifdef IOT_HTTP_SERVER_TESTS

#include <iostream>

#include "../UriParser.h"

//#define BOOST_TEST_MODULE UriParserTest
#include <boost/test/unit_test.hpp>

using namespace saba::web;

BOOST_AUTO_TEST_CASE(UriParserTest_path1)
{
  UriParser uriParser("/");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/");
  BOOST_CHECK(uriParser.getFragment().empty());
  BOOST_CHECK_EQUAL(uriParser.params().size(), 0);
}

BOOST_AUTO_TEST_CASE(UriParserTest_path2)
{
  UriParser uriParser("/abc/def");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/abc/def");
  BOOST_CHECK(uriParser.getFragment().empty());
  BOOST_CHECK_EQUAL(uriParser.params().size(), 0);
}

BOOST_AUTO_TEST_CASE(UriParserTest_fragment)
{
  UriParser uriParser("/url#test");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/url");
  BOOST_CHECK_EQUAL(uriParser.getFragment(), "test");
  BOOST_CHECK_EQUAL(uriParser.params().size(), 0);
}

BOOST_AUTO_TEST_CASE(UriParserTest_params1)
{
  UriParser uriParser("/abc/def?a=b");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/abc/def");
  BOOST_CHECK(uriParser.getFragment().empty());
  BOOST_CHECK_EQUAL(uriParser.params().size(), 1);

  auto it = uriParser.params().find("a");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "b");
}

BOOST_AUTO_TEST_CASE(UriParserTest_params2)
{
  UriParser uriParser("/?name=value&name2=1234");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/");
  BOOST_CHECK(uriParser.getFragment().empty());
  BOOST_CHECK_EQUAL(uriParser.params().size(), 2);

  auto it = uriParser.params().find("name");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "value");

  it = uriParser.params().find("name2");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "1234");
}

BOOST_AUTO_TEST_CASE(UriParserTest_params_fragment)
{
  UriParser uriParser("/my/uri/?arg1=value1&arg2=xyz#qwertz");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/my/uri/");
  BOOST_CHECK_EQUAL(uriParser.getFragment(),"qwertz");
  BOOST_CHECK_EQUAL(uriParser.params().size(), 2);

  auto it = uriParser.params().find("arg1");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "value1");

  it = uriParser.params().find("arg2");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "xyz");
}

BOOST_AUTO_TEST_CASE(UriParserTest_decode)
{
  UriParser uriParser("/my/uri/?arg1=hello%20world&arg2=%5c%26%3D#q%3Bw%3Ae%2A%23");

  BOOST_CHECK_EQUAL(uriParser.getPath(), "/my/uri/");
  BOOST_CHECK_EQUAL(uriParser.getFragment(), "q;w:e*#");
  BOOST_CHECK_EQUAL(uriParser.params().size(), 2);

  auto it = uriParser.params().find("arg1");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "hello world");

  it = uriParser.params().find("arg2");
  BOOST_REQUIRE(it != uriParser.params().end());
  BOOST_CHECK_EQUAL(it->second, "\\&=");
}

#endif



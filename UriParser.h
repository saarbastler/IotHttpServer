#ifndef _INCLUDE_URI_PARSER_H_
#define _INCLUDE_URI_PARSER_H_

#include <string>
#include <unordered_map>

#include <boost/utility/string_view.hpp>

#include "SabaException.h"

namespace saba
{
  namespace web
  {
    class UriParser
    {
    public:

      //template<class string_type>
      UriParser(boost::string_view uri)
      {
        auto it = uri.begin();
        for (; it != uri.end(); it++)
          if (*it == '?' || *it == '#')
          {
            path = std::move(std::string(uri.begin(), it));
            break;
          }

        if (it == uri.end())
        {
          path = std::move( std::string(uri.begin(), uri.end()));
        }
        else if (*it == '?')
        {
          do
          {
            auto start = ++it;
            std::string key;

            for (; it != uri.end(); it++)
              if (*it == '=')
              {
                key = std::move(std::string(start, it));
                break;
              }

            if (it == uri.end())
              throw saba::Exception("missing '=' after '?%s'", start);

            start = ++it;
            for (; it != uri.end(); it++)
              if (*it == '&' || *it == '#')
                break;

            std::string value = std::move(std::string(start, it));
            urlDecode(key);
            urlDecode(value);
            arguments.emplace(std::move(key), std::move(value));

          } while (it != uri.end() && *it == '&');
        }

        if (it != uri.end() && *it == '#')
        {
          fragment = std::move(std::string(it + 1, uri.end()));
          urlDecode(fragment);
        }
      }

      static char fromHex(char c)
      {
        if (c >= 'a')
          return 10 + c - 'a';
        else if (c >= 'A')
          return 10 + c - 'A';
        else
          return c - '0';
      }

      static void urlDecode(std::string& text)
      {
        std::string result;
        result.reserve(text.size());
        for (auto it = text.begin(); it != text.end(); it++)
          if (*it == '%')
          {
            if (++it == text.end())
              throw saba::Exception("illegal url sequence");
            if (!isxdigit(*it))
              throw saba::Exception("illegal url sequence: %%%c", *it);

            char c = fromHex(*it) << 4;

            if (++it == text.end())
              throw saba::Exception("illegal url sequence");
            if (!isxdigit(*it))
              throw saba::Exception("illegal url sequence: %%%c", *it);

            c |= fromHex(*it);

            result += c;
          }
          else
          {
            result += *it;
          }

        result.swap(text);
      }

      const std::string& getPath() const
      {
        return path;
      }

      const std::string& getFragment() const
      {
        return fragment;
      }

      using Parameter = std::unordered_map<std::string, std::string>;

      const Parameter& params() const
      {
        return arguments;
      }

    private:
      std::string path;
      Parameter arguments;
      std::string fragment;
    };
  }
}

#endif // !_INCLUDE_URI_PARSER_H_


#ifndef _INCLUDE_CONFIG_H_
#define _INCLUDE_CONFIG_H_

#include <unordered_map>
#include <ctype.h>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

namespace saba
{
  namespace web
  {
    constexpr const char *ADDRESS = "config.address";
    constexpr const char *PORT = "config.port";
    constexpr const char *THREADS = "config.threads";
    constexpr const char *MIMETYPES = "config.mimetypes";
    constexpr const char *DEFAULT_MIME = "config.defaultmimetype";
    constexpr const char *EXTENSION = "extension";
    constexpr const char *TYPE = "type";
    constexpr const char *DIRECTORIES = "config.directories";
    constexpr const char *URI = "uri";
    constexpr const char *PATH = "path";
    constexpr const char *DEFAULT = "default";

    class Config
    {
    public:

      template<class Callback>
      void read(const std::string& file, Callback directoryCallback)
      {
        boost::property_tree::ptree tree;

        boost::property_tree::read_xml(file, tree);
        //boost::property_tree::write_xml(std::cout, tree);

        address = tree.get<std::string>(ADDRESS);
        port = tree.get<unsigned>(PORT);
        threads = tree.get<unsigned>(THREADS);
        defaultMime = tree.get<std::string>(DEFAULT_MIME);

        mimetypes.clear();
        for (auto it : tree.get_child(MIMETYPES))
          mimetypes[it.second.get<std::string>(EXTENSION)]= it.second.get<std::string>(TYPE);

        readMore(tree);

        for (auto it : tree.get_child(DIRECTORIES))
          directoryCallback(it.second.get<std::string>(URI), it.second.get<std::string>(PATH), it.second.get<std::string>(DEFAULT));
      }

      const std::string& getAddress()
      {
        return address;
      }

      unsigned getPort()
      {
        return port;
      }

      unsigned getThreads()
      {
        return threads;
      }

      const std::string& operator[] (const boost::filesystem::path& extension)
      {
        std::string utf_ext(boost::locale::conv::utf_to_utf<char>(extension.c_str()));
        std::transform(utf_ext.begin(), utf_ext.end(), utf_ext.begin(), tolower);

        if (!utf_ext.empty() && utf_ext[0] == '.')
          utf_ext = utf_ext.substr(1);

        auto result = mimetypes.find(utf_ext);
        if (result == mimetypes.end())
          return defaultMime;
        else
          return result->second;
      }

    protected:

      virtual void readMore(boost::property_tree::ptree& tree) {}

      std::string address;
      unsigned port;
      unsigned threads;

      std::string defaultMime;
      std::unordered_map<std::string, std::string> mimetypes;
    };
  }
}

#endif // !_INCLUDE_CONFIG_H_


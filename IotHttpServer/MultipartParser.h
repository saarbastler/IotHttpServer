#ifndef _INCLUDE_MULTI_PART_PARSER_
#define _INCLUDE_MULTI_PART_PARSER_

#include <boost/utility/string_view.hpp>
#include <boost/algorithm/string.hpp>

#include "SabaException.h"

namespace saba
{
  namespace web
  {
    using namespace boost;

    class MultipartParser
    {
    public:

      MultipartParser(string_view body)
      {
        string_view::iterator it = body.begin();
        string_view separator = readToEndOfLine(it, body.end());

        while (it != body.end())
        {
          string_view line = readToEndOfLine(it, body.end());

          if (line.starts_with(ContentDisposition))
            readFilename(line);
          else if (line.starts_with(ContentType))
            readContenttype(line);
          else if (line.empty())
          {
            fileStart = it;
            break;
          }
        }

        if (fileStart)
        {
          size_t end = body.find(separator, fileStart - body.begin());
          fileSize = end - (fileStart - body.begin());

          if (fileStart[fileSize - 1] == '\n')
            fileSize--;
          if (fileStart[fileSize - 1] == '\r')
            fileSize--;
        }
        else
          throw saba::Exception("unable to parse Multipart Body");
      }

      const std::string& getFilename()
      {
        return filename;
      }

      const std::string& getContentType()
      {
        return contentType;
      }

      const char *getFileStart()
      {
        return fileStart;
      }

      size_t getFilesize()
      {
        return fileSize;
      }

    protected:

      static constexpr const char *ContentDisposition= "Content-Disposition:" ;
      static constexpr const char *FileName = "filename=\"";
      static constexpr const char *ContentType = "Content-Type:";

      std::string filename;
      std::string contentType;
      const char *fileStart= nullptr;
      size_t fileSize = 0;

      void readContenttype(string_view line)
      {
        contentType = std::string(line.begin() + strlen(ContentType), line.end());
        boost::trim(contentType);
      }

      void readFilename(string_view line)
      {
        size_t filenamePos = line.find(FileName);
        if (filenamePos != string_view::npos)
        {
          filenamePos += strlen(FileName);
          size_t filenameEnd = line.find('\"', filenamePos);
          if (filenameEnd != string_view::npos)
            filename = std::string(line.begin() + filenamePos, filenameEnd - filenamePos);
        }
      }

      string_view readToEndOfLine(string_view::iterator& it, string_view::iterator end)
      {
        string_view::iterator from = it;
        for(;it != end;it++)
        { 
          if (*it == '\r')
          {
            string_view::iterator to = it;
            it++;
            if (*it == '\n')
              it++;

            return string_view(from, to-from);
          }
          else if(*it == '\n')
          {
            string_view::iterator to = it;
            it++;

            return string_view(from, to - from);
          }
        }

        return string_view();
      }
    };
  }
}

#endif // _INCLUDE_MULTI_PART_PARSER_


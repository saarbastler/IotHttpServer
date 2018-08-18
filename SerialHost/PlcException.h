#ifndef _INCLUDE_PLC_EXCEPTION_H
#define _INCLUDE_PLC_EXCEPTION_H

#include <exception>
#include <cstdarg>
#include <cstdio>
#include <string>

class PlcException : public std::exception
{
public:

  PlcException(const PlcException& other)
  {
    text = other.text;
  }

  PlcException(PlcException&& other)
  {
    text.swap(other.text);
  }

  PlcException(const char *format, ...)
  {
    va_list args;
    va_start(args, format);
    char buffer[1024];

    std::vsnprintf(buffer, sizeof(buffer)/sizeof(buffer[0]), format, args);
    va_end(args);

    text = buffer;
  }

  virtual const char* what() const noexcept
  {
    return text.c_str();
  }

private:

  std::string text;
};
#endif // _INCLUDE_PLC_EXCEPTION_H


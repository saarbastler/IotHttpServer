#ifndef _INCLUDE_SABA_EXCEPTION_H_
#define _INCLUDE_SABA_EXCEPTION_H_

#include <exception>
#include <cstdarg>

namespace saba
{
  class Exception : public std::exception
  {
  public:
    Exception(const char *format, ...)
    {
      va_list args;
      va_start(args, format);

      char buffer[1000];
      vsnprintf(buffer, 1000, format, args);
      va_end(args);

      message = buffer;
    }

    virtual const char *what()
    {
      return message.c_str();
    }

  private:
    std::string message;
  };

}

#endif // !_INCLUDE_SABA_EXCEPTION_H_


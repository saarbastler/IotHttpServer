#ifndef _INCLUDE_COMMA_H_
#define _INCLUDE_COMMA_H_

#include <ostream>

class Comma
{
public:

  bool operator()()
  {
    bool temp = comma;
    comma = true;

    return temp;
  }

private:

  bool comma = false;
};

inline std::ostream& operator << (std::ostream& out, Comma& comma)
{
  if (comma())
    out << ',';

  return out;
}

#endif // !_INCLUDE_COMMA_H_


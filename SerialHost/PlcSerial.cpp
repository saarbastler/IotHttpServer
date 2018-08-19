#include <iostream>

#include "PlcSerial.h"

void PlcSerial::dataReceived(const char *readBuffer, unsigned count)
{
  const char *src = readBuffer;
  unsigned readIndex;
  for (readIndex=0; writeIndex < BUFFER_SIZE && readIndex < count;readIndex++)
  {
    char ch = *src++;

    if (ch == '\r')
      continue;
    if (ch == '\n')
    {
      buffer[writeIndex] = 0;

      lineComplete();
      writeIndex = 0;

      continue;
    }

    buffer[writeIndex++] = ch;
  }
}

void PlcSerial::lineComplete()
{
  bool error = true;
  unsigned index = 1;
  switch (buffer[0])
  {
  case'I':
    error= handleIO(EventType::Input);
    break;

  case 'O':
    error = handleIO(EventType::Output);
    break;

  case 'M':
    if (isdigit(buffer[index]))
    {
      unsigned num = readNumber(index);

      if (buffer[index++] == ':' && isdigit(buffer[index]))
      {
        bool value = buffer[index++] != '0';
        if (buffer[index++] == ',' && isdigit(buffer[index]))
        {
          unsigned duration = readNumber(index);
          if (buffer[index++] == ',' && isdigit(buffer[index]))
          {
            unsigned remaining = readNumber(index);
            EventMonoflop event(EventType::Monoflop, num, value, duration, remaining);

            fireEvent(event);
            error = false;
          }
        }
      }
    }
    break;

  }

  if (error)
  {
    ErrorEvent error(buffer);

    fireEvent(error);
  }
}

bool PlcSerial::handleIO(EventType eventType)
{
  bool error = true;
  unsigned index = 1;

  if (isdigit(buffer[index]))
  {
    unsigned num = readNumber(index);

    if (buffer[index++] == ':')
    {
      if (isdigit(buffer[index]))
      {
        ValueEvent valueEvent(eventType, num, buffer[index] != '0');

        fireEvent(valueEvent);
        return false;
      }
    }
  }
  else if (buffer[index] == ':')
  {
    readBinaryAndFire(index + 1, eventType);
    return false;
  }

  return true;
}

void PlcSerial::readBinaryAndFire(unsigned index, EventType eventType)
{
  unsigned eventIndex = 0;
  while (isdigit(buffer[index]))
  {
    ValueEvent event(eventType, eventIndex++, buffer[index++] != '0');
    
    fireEvent(event);
  }
}

unsigned PlcSerial::readNumber(unsigned& index)
{
  unsigned result = 0;
  while (isdigit(buffer[index]))
  {
    result *= 10;
    result += (buffer[index] - '0');
    index++;
  }

  return result;
}

void PlcSerial::fireEvent(Event& event)
{
  if (!initialized)
  {
    send("E0\nO\nM\n", 7);
    initialized = true;
  }

  std::cout << event;

  for (auto it : listener)
    it(event);

  if( ValueEvent *valueEvent=dynamic_cast<ValueEvent*>(&event))
    if( valueEvent->getNum() == 7)
      send("M0S\r\n", 5);

}
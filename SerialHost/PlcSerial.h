#ifndef _INCLUDE_PLC_SERIAL_H_
#define _INCLUDE_PLC_SERIAL_H_

#include <functional>
#include <vector>

#include "Serial.h"
#include "PlcEvent.h"

class PlcSerial :  public Serial
{
public:
  static constexpr unsigned BUFFER_SIZE = 80;

  PlcSerial(std::shared_ptr<boost::asio::io_service> io, unsigned bufferSize) : Serial(io, bufferSize) {}

  virtual ~PlcSerial() {}

  typedef std::function<void(Event&)> EventListener;

  void addEventListener(EventListener eventListener)
  {
    listener.emplace_back();
    listener.back() = eventListener;
  }

  void close()
  {
    listener.clear();
    Serial::close();
  }

protected:

  virtual void dataReceived(const char *buffer, unsigned count);

  void lineComplete();

private:

  void fireEvent(Event& event);

  std::vector<EventListener> listener;

  unsigned readNumber(unsigned& index);
  void readBinaryAndFire(unsigned index, EventType eventType);
  bool handleIO(EventType eventType);

  bool initialized = false;
  unsigned writeIndex = 0;
  char buffer[BUFFER_SIZE];
};

#endif // !_INCLUDE_PLC_SERIAL_H_

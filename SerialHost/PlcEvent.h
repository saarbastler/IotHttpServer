#ifndef _INCLUDE_PLC_EVENT_H_
#define _INCLUDE_PLC_EVENT_H_

#include <string>
#include <ostream>

enum class EventType { Error, Input, Output, Monoflop, Merker, Status };

std::ostream& operator << (std::ostream& out, EventType eventType);

class Event
{
private:
  EventType eventType;

public:

  Event(EventType eventType) : eventType(eventType) {}

  EventType getEventType()
  {
    return eventType;
  }

  virtual void dump(std::ostream& out)
  {
    out << getEventType() << std::endl;
  }

};

std::ostream& operator << (std::ostream& out, Event& event);

class ErrorEvent : public Event
{
private:
  std::string text;

public:

  ErrorEvent(const char *text) : Event(EventType::Error), text(text) {}

  const char *getText()
  {
    return text.c_str();
  }

  virtual void dump(std::ostream& out)
  {
    out << "Error: " << getText() << std::endl;
  }

};

class ValueEvent : public Event
{
private:
  unsigned num;
  bool value;

public:

  ValueEvent(EventType eventType, unsigned num, bool value) : Event(eventType), num(num), value(value) {}

  unsigned getNum()
  {
    return num;
  }

  bool getValue()
  {
    return value;
  }

  virtual void dump(std::ostream& out)
  {
    out << getEventType() << ' ' << getNum() << '=' << getValue() << std::endl;
  }

};

class EventMonoflop : public ValueEvent
{
private:
  unsigned duration;
  unsigned remaining;

public:

  EventMonoflop(EventType eventType, unsigned num, bool value, unsigned duration, unsigned remaining)
    : ValueEvent(eventType, num, value), duration(duration), remaining(remaining) {}

  unsigned getDuration()
  {
    return duration;
  }

  unsigned getRemaining()
  {
    return remaining;
  }

  virtual void dump(std::ostream& out)
  {
    out << getEventType() << ' ' << getNum() << '=' << getValue()
      << " Duration: " << (getDuration() * 2) << " s Remaining: " << (getRemaining() * 2)
      << " s" << std::endl;
  }
};

class StatusEvent : public Event
{
private:
  unsigned status;

public:

  StatusEvent(unsigned status) : Event(EventType::Status), status(status) {}

  unsigned getStatus()
  {
    return status;
  }

  virtual void dump(std::ostream& out)
  {
    out << "Status: " << getStatus() << std::endl;
  }
};

#endif // _INCLUDE_PLC_EVENT_H_

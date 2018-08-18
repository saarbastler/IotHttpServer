#include "PlcEvent.h"

std::ostream& operator << (std::ostream& out, EventType eventType)
{
  static char *eventTypeText[] = { "Error", "Input", "Output", "Monoflop", "Merker" };

  int index = static_cast<unsigned>(eventType);
  if (index > static_cast<unsigned>(EventType::Merker))
    out << "undefined EventType: " << index;
  else
    out << eventTypeText[index];

  return out;
}


std::ostream& operator << (std::ostream& out, Event& event)
{
  event.dump(out);

  return out;
}

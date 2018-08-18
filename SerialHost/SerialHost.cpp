// SerialHost.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "SerialHost.h"

#include <iostream>

void SerialHost::open(const char *comPort, unsigned baudrate)
{
  serial.open(comPort, baudrate);
  serial.addEventListener(std::bind(&SerialHost::eventListener, this, std::placeholders::_1));
}

void SerialHost::close()
{
  std::cout << "SerialHost close called" << std::endl;

  serial.close();
}

void SerialHost::eventListener(Event& event)
{
  using namespace saba::plc;

  static const DataType typeIndexFromEventType[] = { DataType::Inputs, DataType::Outputs, DataType::Monoflops, DataType::Merker };

  if( event.getEventType() != EventType::Error)
    try
    {
    DataType dataType = typeIndexFromEventType[static_cast<unsigned>(event.getEventType()) - 1];
      if (ValueEvent *valueEvent = dynamic_cast<ValueEvent*>(&event))
        plcModel.set(dataType, valueEvent->getNum(), valueEvent->getValue());
    }
    catch (std::exception& ex)
    {
      std::cout << "Exception: " << ex.what() << std::endl;

    }
}
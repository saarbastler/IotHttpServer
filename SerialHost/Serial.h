/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Serial.h
 * Author: Joerg
 *
 * Created on 1. Mai 2017, 19:46
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <boost/asio.hpp>
#include <vector>
#include <memory>

#include "PlcException.h"

class Serial
{
public:
  Serial(boost::asio::io_service& io, unsigned bufferSize) : serial(io), readMem(bufferSize)
  {
  }
  
  void open(const char *portName, unsigned baudrate)
  {
    using namespace boost::asio;
    
    try
    {
      serial.open(portName);
      serial.set_option(serial_port::baud_rate(baudrate));
      serial.set_option(serial_port::flow_control(serial_port::flow_control::none));
      serial.set_option(serial_port::parity(serial_port::parity::none));
      serial.set_option(serial_port::stop_bits(serial_port::stop_bits::one));
      serial.set_option(serial_port::character_size(8));

      successfulOpened();

      readAsync();
    }
    catch (std::exception& ex)
    {
      throw PlcException("unable to open %s: %s", portName, ex.what());
    }
  }
  
  void close()
  {
    serial.close();
  }

private:
  
  void readAsync()
  {
    serial.async_read_some(boost::asio::buffer(readMem), [this](const boost::system::error_code& error,std::size_t bytes_transferred)
    {
      dataReceived(&readMem[0], (unsigned)bytes_transferred);
      
      if(!error)
        readAsync();
    });    
  }
  
protected:

  void send(const char *buffer, int count)
  {
    boost::asio::write( serial, boost::asio::buffer(buffer, count));
  }
  
  virtual void dataReceived(const char *buffer, unsigned count) = 0;

  virtual void successfulOpened()
  {
  }

private:

  boost::asio::serial_port serial;

  std::vector<char> readMem;
};

#endif /* SERIAL_H */


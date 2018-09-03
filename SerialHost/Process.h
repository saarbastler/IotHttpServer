#ifndef _INCLUDE_PROCESS_H_
#define _INCLUDE_PROCESS_H_

#include <iostream>

#include <boost/process.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <base.hpp>

class Process
{
public:
  Process(boost::asio::io_service& ios) : ios(ios) {  }

  bool isRunning()
  {
    return running;
  }

  template<class Callback>
  void startProcess(std::shared_ptr<http::base::connection> connection, const std::string& commandline,
    const Callback& handler, void* arg= nullptr)
  {
    if (!running)
    {
      argument = arg;
      running = true;

      tcpConnection.swap(connection);
      pipe.reset(new boost::process::async_pipe(ios));

      boost::process::async_system(
        ios,
        [this, &handler](boost::system::error_code ec, int code)
      {
        std::cout << "ExitHandler : " << ec << " :" << code << std::endl;

        tcpConnection->shutdown();
        tcpConnection.reset();

        pipe->close();
        pipe.reset();

        running = false;

        handler(argument);
      },
        commandline,
        boost::process::std_out > boost::process::null,
        boost::process::std_err > *pipe
        );

      pipe->async_read_some(boost::asio::buffer(buffer, BufferSize),
        boost::bind(&Process::readHandler, this,
          boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
    }
  }

protected:

  void readHandler(
    const boost::system::error_code& error, // Result of operation.
    std::size_t bytes_transferred           // Number of bytes read.
  )
  {
    if (!error && bytes_transferred)
    {
      tcpConnection->stream().async_write_some(boost::asio::buffer(buffer, bytes_transferred), 
        [this](const boost::system::error_code& error,
        std::size_t bytes_transferred)
      {
        //std::cout << "Bytes read: " << bytes_transferred << std::endl;

        if (!error && bytes_transferred)
        {
          pipe->async_read_some(boost::asio::buffer(buffer, 100),
            boost::bind(&Process::readHandler, this,
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
        }
        else
        {
          std::cout << "Write Error: " << error << std::endl;
        }
      });

    }
    else
    {
      std::cout << "Read Error: " << error << std::endl;
    }
  }

private:
  static constexpr unsigned BufferSize = 1000;
  char buffer[BufferSize];

  boost::asio::io_service& ios;
  std::shared_ptr<boost::process::async_pipe> pipe;
  std::shared_ptr<http::base::connection> tcpConnection;

  void *argument = nullptr;
  volatile bool running = false;
};

#endif // !_INCLUDE_PROCESS_H_


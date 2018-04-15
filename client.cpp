//
// connect_pair.cpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <cctype>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <string>     // std::string, std::stoi, std::stol
#include <fstream>
#include <array>
#include <cstring>

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

enum cmd {Enable = 1, Add, Remove, GetCount, GetValue, Disable};

#define stop 0
bool enter_critical = true;

using boost::asio::local::stream_protocol;



std::array<std::string, 12> myInput = {

		"1 1",
		"3 2 10 5000",
		"1 4",
		"3 2 11 11000",
		"1 4",
		"3 2 10 10000",
		"1 4",
		"2 5 10",
		"2 5 11",
		"2 3 11",
		"1 4",
		"1 6"
};


class uppercase_filter
{
public:
  uppercase_filter(boost::asio::io_service& io_service)
    : socket_(io_service)
  {
  }

  stream_protocol::socket& socket()
  {
    return socket_;
  }

  void start()
  {
    // Wait for request.
    socket_.async_read_some(boost::asio::buffer(data_),
        boost::bind(&uppercase_filter::handle_read,
          this, boost::asio::placeholders::error,
          boost::asio::placeholders::bytes_transferred));
  }


  void ProcessCommand(std::string buffer, size_t size)
  {

	  int err = 1;
	  enter_critical = 0;

	  //Check machine's endianness
	  unsigned int i = 1;
	  char *c = (char*)&i;
	  if (*c)
	  	std::cout << "Little endian" << std::endl;
	  else
		  std::cout << "Big endian" << std::endl;

	  if(strlen(buffer.c_str()) == size)//number of values
	  {
			  enter_critical = 1;
			  err = 0;
	  }

	  if(err)
	  {
		  std::ofstream error;
		  error.open("error.log", std::ofstream::out | std::ofstream::app);
		  error << buffer;
		  throw "Invalid Command";
	  }

  }

  int command(const char *cmd, char  *Result)
  {

	  int x         = cmd[2] - '0';//type of cmd
	  int dummy     = 0;

	  std::ostringstream convert;
	  std::string server(1,'1');
	  std::string str;

	  switch(x){

	  case 1:
		  enable();
		  break;
	  case 2:
		  add(cmd);
		  break;
	  case 3:
		  remove(cmd);
		  break;
	  case 4:
		  dummy  = getCount();
		  convert << dummy;
		  str = server + convert.str();
		  std::strcpy(Result,str.c_str());
		  break;
	  case 5:
		  dummy = getValue(cmd);
		  convert << dummy;
		  str = server + convert.str();
		  std::strcpy(Result,str.c_str());
		  break;
	  case 6:
		  disable();
		  break;
	  }

	  if(dummy)
		  return dummy;
	  else
		  return 0;
  }

  int SendResponse(const char *reply, long int size)
  {

   int begin = 1;
   char  *Result;
   int cmd_type = command(reply, Result);
   std::string str(Result);

	  if(cmd_type)
	  {

		// Show reply to user.
		std::cout << "Result: ";
		std::cout << str << std::endl;
		//std::cout.write(reply, size);
	  }
	  else if(cmd::Disable == cmd_type)
		  begin = 0;

  return begin;

  }


private:


  int enable(void)
  {
	  return 1;
  }

  void add(const char *cmd)
  {
	  std::string myString(cmd);

	  std::string dummy1 = myString.substr(4, 2);
	  int key            = std::atoi(dummy1.c_str());

	  std::string dummy2 = myString.substr(7);
	  int value = std::atoi(dummy2.c_str());

	  unsigned int index = key;
	  myBuffer[index] = value;

  }

  void remove(const char *cmd)
  {

	  std::string myString(cmd);
	  std::string dummy1 = myString.substr(4, 2);
	  int key            = std::atoi(dummy1.c_str());

	  for(auto it = myBuffer.begin(); it != myBuffer.end();  ++it)
	  {
		   if(it->first  == key)
			   it = myBuffer.erase(it);

	  }

  }


  int getCount()
  {
	  return myBuffer.size();
  }

  int getValue(const char *cmd)
  {

	  std::string myString(cmd);
	  std::string dummy1 = myString.substr(4, 2);
	  int key            = std::atoi(dummy1.c_str());

	 auto it = myBuffer.find(key);
	    if (it != myBuffer.end())
	    	return it->second;
	    else
	    	return -1;

  }

  int disable(void)
  {

	  return 0;

  }



  void handle_read(const boost::system::error_code& ec, std::size_t size)
  {
    if (!ec)
    {
      // Compute result.
      for (std::size_t i = 0; i < size; ++i)
        data_[i] = std::toupper(data_[i]);

      // Send result.
      boost::asio::async_write(socket_, boost::asio::buffer(data_, size),
          boost::bind(&uppercase_filter::handle_write,
            this, boost::asio::placeholders::error));
    }
    else
    {
      throw boost::system::system_error(ec);
    }
  }

  void handle_write(const boost::system::error_code& ec)
  {
    if (!ec)
    {
      // Wait for request.
      socket_.async_read_some(boost::asio::buffer(data_),
          boost::bind(&uppercase_filter::handle_read,
            this, boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }
    else
    {
      throw boost::system::system_error(ec);
    }
  }

  stream_protocol::socket socket_;
  boost::array<char, 512> data_;
  std::map<int,int> myBuffer;
};

void run(boost::asio::io_service* io_service)
{
  try
  {
    io_service->run();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception in thread: " << e.what() << "\n";
    std::exit(1);
  }
}

int main()
{

  int i = 0;

  try
  {
    boost::asio::io_service io_service;

    // Create filter and establish a connection to it.
    uppercase_filter filter(io_service);
    stream_protocol::socket socket(io_service);
    boost::asio::local::connect_pair(socket, filter.socket());
    filter.start();

    // The io_service runs in a background thread to perform filtering.
    boost::thread thread(boost::bind(run, &io_service));

	  for (i=0; i<12; i++)
    {

	  std::string request(myInput[i]);

	  // Process user's input
      filter.ProcessCommand(request, request.size());

      if(enter_critical)
      {
		  // Send request to filter.
		  boost::asio::write(socket, boost::asio::buffer(request));

		  // Wait for reply from filter.
		  std::vector<char> reply(request.size());
		  boost::asio::read(socket, boost::asio::buffer(reply));

		  if(filter.SendResponse(&reply[0], request.size()) == stop)
			  break;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
    std::exit(1);
  }
}

#else // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
# error Local sockets not available on this platform.
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

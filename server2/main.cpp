//
// main.cpp
// ~~~~~~~~
//
// Copyright (c) 2003-2024 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include "server.hpp"
#include "globals.hpp"

#include <async_mqtt5.hpp>

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;
client_type* pmqtt;

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 5)
    {
      std::cerr << "Usage: http_server <address> <port> <threads> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 1 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 1 .\n";
      return 1;
    }

    // Initialise the server.
    std::size_t num_threads = std::stoi(argv[3]);
    http::server2::server s(argv[1], argv[2], argv[4], num_threads);

    boost::asio::io_context iocMqtt;

    std::thread mqtt_thread([&iocMqtt] {
        client_type c(iocMqtt);

        pmqtt = &c;

        c.credentials("client-id", "servercpp", "server12345")
        .brokers("127.0.0.1", 1883)
        .async_run(boost::asio::detached);

        iocMqtt.run();
    });

    mqtt_thread.detach(); 

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}

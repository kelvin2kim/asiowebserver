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
#include <boost/beast/http.hpp>
#include "server.hpp"

#include "globals.hpp"

#include <async_mqtt5.hpp>

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;
client_type* pmqtt;

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 4)
    {
      std::cerr << "Usage: http_server <address> <port> <doc_root>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    receiver 0.0.0.0 80 .\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    receiver 0::0 80 .\n";
      return 1;
    }

    // Initialise the server.
    http::server::server s(argv[1], argv[2], argv[3]);

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

    mongocxx::v_noabi::instance inst{};
    mongocxx::uri uri("mongodb://127.0.0.1:27017");
    mongocxx::client client(uri);


    mongocxx::database db = client["admin"];
    mongocxx::collection coll = db["test"];

    if(!client){
        std::cout<<"db is not started";
      }
    else{
      std::cout<<"db is  started";
    }

    bsoncxx::builder::stream::document document{};
    document << "Hello" << "world";

    coll.insert_one(document.view());


    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}

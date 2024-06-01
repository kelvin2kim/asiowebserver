#include <iostream>
#include <string>
#include <boost/asio.hpp>
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

#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <pqxx/pqxx>
#include <boost/json.hpp>

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;
client_type* pmqtt;

namespace json = boost::json;

constexpr auto use_nothrow_awaitable = boost::asio::as_tuple(boost::asio::use_awaitable);

pqxx::connection* db_connection;

boost::asio::awaitable<bool> subscribe(client_type& client) {
	// Configure the request to subscribe to a Topic.
	async_mqtt5::subscribe_topic sub_topic = async_mqtt5::subscribe_topic{
		"uplink",
		async_mqtt5::subscribe_options {
			async_mqtt5::qos_e::exactly_once, // All messages will arrive at QoS 2.
			async_mqtt5::no_local_e::no, // Forward message from Clients with same ID.
			async_mqtt5::retain_as_published_e::retain, // Keep the original RETAIN flag.
			async_mqtt5::retain_handling_e::send // Send retained messages when the subscription is established.
		}
	};

	// Subscribe to a single Topic.
	auto&& [ec, sub_codes, sub_props] = co_await client.async_subscribe(
		sub_topic, async_mqtt5::subscribe_props {}, use_nothrow_awaitable
	);
	// Note: you can subscribe to multiple Topics in one mqtt_client::async_subscribe call.

	// An error can occur as a result of:
		//  a) wrong subscribe parameters
		//  b) mqtt_client::cancel is called while the Client is in the process of subscribing
	if (ec)
		std::cout << "Subscribe error occurred: " << ec.message() << std::endl;
	else
		std::cout << "Result of subscribe request: " << sub_codes[0].message() << std::endl;

	co_return !ec && !sub_codes[0]; // True if the subscription was successfully established.
}

boost::asio::awaitable<void> subscribe_and_receive(client_type& client) {
	// Configure the Client.
	// It is mandatory to call brokers() and async_run() to configure the Brokers to connect to and start the Client.
	client.brokers("127.0.0.1", 1883) // Broker that we want to connect to. 1883 is the default TCP port.
		.async_run(boost::asio::detached); // Start the client.

	// Before attempting to receive an Application Message from the Topic we just subscribed to,
	// it is advisable to verify that the subscription succeeded.
	// It is not recommended to call mqtt_client::async_receive if you do not have any
	// subscription established as the corresponding handler will never be invoked.
	if (!(co_await subscribe(client)))
		co_return;

	for (;;) {
		// Receive an Appplication Message from the subscribed Topic(s).
		auto&& [ec, topic, payload, publish_props] = co_await client.async_receive(use_nothrow_awaitable);

		if (ec == async_mqtt5::client::error::session_expired) {
			// The Client has reconnected, and the prior session has expired.
			// As a result, any previous subscriptions have been lost and must be reinstated.
			if (co_await subscribe(client))
				continue;
			else
				break;
		} else if (ec)
			break;

    //std::cout << "test" << std::endl;

    try {
            // Parse the JSON payload
            json::value jv = json::parse(payload);
            json::object jo = jv.as_object();
            std::string up_time = json::value_to<std::string>(jo["up_time"]);
            std::string username = json::value_to<std::string>(jo["username"]);
            std::string location = json::value_to<std::string>(jo["location"]);

            // Connect to PostgreSQL and insert the data
            if (db_connection->is_open()) {
                pqxx::work W(*db_connection);
                 std::string sql = "INSERT INTO volunteer (username, up_time, location) VALUES ("
                                  + W.quote(username) + ", "
                                  + W.quote(up_time) + ", "
                                  + "POINT(" + W.quote(location) + ")"
                                  + ") ON CONFLICT (username) DO UPDATE SET "
                                  + "up_time = EXCLUDED.up_time, "
                                  + "location = EXCLUDED.location;";
                W.exec(sql);
                W.commit();
                std::cout << "Volunteer entry created successfully" << std::endl;
            } else {
                std::cout << "Can't open database" << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    
    /*
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
      document << topic << payload;

      coll.insert_one(document.view());
      */
	}

	co_return;
}

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

     db_connection = new pqxx::connection("dbname=postgres user=postgres.zynowokrcwkfqaiaijqr password=!Sunghoon0101 host=aws-0-us-east-1.pooler.supabase.com port=5432");
        if (!db_connection->is_open()) {
            std::cerr << "Can't open database" << std::endl;
            return 1;
        }
        else {
            std::cout << "Opened database successfully" << std::endl;
        }

    // Initialise the server.
    std::size_t num_threads = std::stoi(argv[3]);
    http::server3::server s(argv[1], argv[2], argv[4], num_threads);


    

     boost::asio::io_context iocMqtt;

    std::thread mqtt_thread([&iocMqtt] {
        client_type c(iocMqtt);

        pmqtt = &c;

         c.credentials("client-id", "servercpp", "server12345")
        .brokers("127.0.0.1", 1883)
        .async_run(boost::asio::detached);

        co_spawn(iocMqtt, subscribe_and_receive(c), boost::asio::detached);

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

  //Clean up the global database connection
    if (db_connection) {
        delete db_connection;
    }


  return 0;
}
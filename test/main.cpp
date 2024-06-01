 #include <iostream>
#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>
#include <async_mqtt5.hpp>
#include <boost/asio.hpp>

int main() {
   
   namespace asio = boost::asio;
   asio::io_context ioc;


   using stream_type = asio::ssl::stream<asio::ip::tcp::socket>;
   asio::ssl::context tls_context(asio::ssl::context::tls_client);


   using client_type = async_mqtt5::mqtt_client<stream_type, asio::ssl::context>;
   client_type c(ioc, std::move(tls_context));


   c.credentials("<your client id>", "servercpp", "server12345")
       .brokers("c1fe1ad1.ala.us-east-1.emqxsl.com", 8883)
       .async_run(boost::asio::detached);


   c.async_publish<async_mqtt5::qos_e::at_most_once>(
       "test", "hello world!",
       async_mqtt5::retain_e::no, async_mqtt5::publish_props {},
       [&c](async_mqtt5::error_code ec) {
           std::cout << ec.message() << std::endl;
           c.cancel(); // close the client
       }
   );


   ioc.run();
}
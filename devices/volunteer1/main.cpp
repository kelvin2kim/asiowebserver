#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <chrono>
#include <ctime>
#include <random>
#include "globals.hpp"
#include <async_mqtt5.hpp>
#include <algorithm>
#include <vector>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <pqxx/pqxx>

namespace json = boost::json;

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;
client_type* pmqtt;

constexpr auto use_nothrow_awaitable = boost::asio::as_tuple(boost::asio::use_awaitable);

// Return current timestamp
std::string current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);
    return std::string(buffer);
}

// random location (lat and long)
std::string generate_random_location() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(-90.0, 90.0);
    std::uniform_real_distribution<> lon_dist(-180.0, 180.0);
    double lat = lat_dist(gen);
    double lon = lon_dist(gen);
    return std::to_string(lat) + "," + std::to_string(lon);
}

boost::asio::awaitable<void> publish_data(client_type& client) {
    while (true) {
        // Simulate publishing data
        std::string up_time = current_timestamp();
        std::string username = "Volunteer" + std::to_string(rand() % 1000);
        std::string location = generate_random_location();

        // Create JSON payload 
        json::object volunteer;
        volunteer["up_time"] = up_time;
        volunteer["username"] = username;
        volunteer["location"] = location;
        std::string payload = json::serialize(volunteer);  // Convert JSON object to string

        // Publish a message
        pmqtt->async_publish<async_mqtt5::qos_e::at_most_once>(
            "uplink", payload,
            async_mqtt5::retain_e::no, async_mqtt5::publish_props{},
            [](async_mqtt5::error_code ec) {
            }
        );

        // Wait for some time before the next publish (simulate device behavior)
        co_await boost::asio::steady_timer(co_await boost::asio::this_coro::executor, std::chrono::seconds(5)).async_wait(use_nothrow_awaitable);
    }
}

int main() {
    try {
        boost::asio::io_context iocMqtt;

        std::thread mqtt_thread([&iocMqtt] {
            client_type c(iocMqtt);

            pmqtt = &c;

            c.credentials("client-id2", "clientcpp", "server12345")
            .brokers("127.0.0.1", 1883)
            .async_run(boost::asio::detached);

            co_spawn(iocMqtt, publish_data(c), boost::asio::detached);

            iocMqtt.run();

            
        });
        mqtt_thread.detach(); 

        while (true){}

    } catch (const std::exception &e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

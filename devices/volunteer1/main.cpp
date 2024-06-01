#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <cmath>
#include <boost/asio.hpp>
#include <boost/json.hpp>
#include <chrono>
#include <ctime>
#include "globals.hpp"
#include <async_mqtt5.hpp>
#include <boost/asio/as_tuple.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>

namespace json = boost::json;

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;
client_type* pmqtt;

constexpr auto use_nothrow_awaitable = boost::asio::as_tuple(boost::asio::use_awaitable);

const double CENTRAL_PARK_LAT_MIN = 40.764356;
const double CENTRAL_PARK_LAT_MAX = 40.800622;
const double CENTRAL_PARK_LON_MIN = -73.973042;
const double CENTRAL_PARK_LON_MAX = -73.949297;

// Return current timestamp
std::string current_timestamp() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm* now_tm = std::localtime(&now_time);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);
    return std::string(buffer);
}

// Generate random initial location within Central Park
std::pair<double, double> generate_initial_location() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(CENTRAL_PARK_LAT_MIN, CENTRAL_PARK_LAT_MAX);
    std::uniform_real_distribution<> lon_dist(CENTRAL_PARK_LON_MIN, CENTRAL_PARK_LON_MAX);
    return {lat_dist(gen), lon_dist(gen)};
}

// Simulate movement within a small radius (10 meters)
std::pair<double, double> move_within_radius(double lat, double lon) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(-0.00009, 0.00009); // Approx. 10 meters in latitude/longitude degrees
    return {lat + dist(gen), lon + dist(gen)};
}

// Calculate the distance between two coordinates in meters
double haversine(double lat1, double lon1, double lat2, double lon2) {
    const double R = 6371000; // Earth's radius in meters
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;
    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = std::sin(dLat / 2) * std::sin(dLat / 2) +
               std::sin(dLon / 2) * std::sin(dLon / 2) * std::cos(lat1) * std::cos(lat2);
    double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1 - a));
    return R * c;
}

std::pair<double, double> generate_random_location() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(CENTRAL_PARK_LAT_MIN, CENTRAL_PARK_LAT_MAX);
    std::uniform_real_distribution<> lon_dist(CENTRAL_PARK_LON_MIN, CENTRAL_PARK_LON_MAX);
    return {lat_dist(gen), lon_dist(gen)};
}

boost::asio::awaitable<void> publish_data(client_type& client, std::vector<std::pair<std::string, std::pair<double, double>>> &volunteers, std::pair<double, double>& beacon_location) {
    while (true) {
        // Move the BLE beacon within a small radius
        beacon_location = generate_random_location();
        std::cout << "Beacon new location: " << beacon_location.first << ", " << beacon_location.second << std::endl;

        for (auto& [username, location] : volunteers) {
            // Move the volunteer within a small radius
            location = move_within_radius(location.first, location.second);
            std::cout << "Volunteer " << username << " new location: " << location.first << ", " << location.second << std::endl;

            // Check if the BLE beacon is within 20 meters of the volunteer
            double distance = haversine(beacon_location.first, beacon_location.second, location.first, location.second);
            std::cout << "Distance to beacon: " << distance << " meters" << std::endl;
            if (distance <= 600) {
                // Create JSON payload
                json::object volunteer;
                volunteer["up_time"] = current_timestamp();
                volunteer["username"] = username;
                volunteer["location"] = std::to_string(location.first) + "," + std::to_string(location.second);
                std::string payload = json::serialize(volunteer);  // Convert JSON object to string

                // Publish a message
                 pmqtt->async_publish<async_mqtt5::qos_e::at_most_once>(
                    "uplink", payload,
                    async_mqtt5::retain_e::no, async_mqtt5::publish_props{},
                    [](async_mqtt5::error_code ec) {
                    }
                );
            }
        }

        // Wait for some time before the next publish (simulate device behavior)
        co_await boost::asio::steady_timer(co_await boost::asio::this_coro::executor, std::chrono::seconds(5)).async_wait(use_nothrow_awaitable);
    }
}

int main() {
    try {
        boost::asio::io_context iocMqtt;

        std::vector<std::pair<std::string, std::pair<double, double>>> volunteers;

        // Generate 10 volunteers with initial locations
        for (int i = 0; i < 10; ++i) {
            std::string username = "Volunteer" + std::to_string(i + 1);
            auto location = generate_initial_location();
            volunteers.push_back({username, location});
            std::cout << "Generated volunteer " << username << " at location: " << location.first << ", " << location.second << std::endl;
        }

        // Generate initial location for the BLE beacon
        std::pair<double, double> beacon_location = generate_initial_location();
        std::cout << "Generated BLE beacon at location: " << beacon_location.first << ", " << beacon_location.second << std::endl;

        std::thread mqtt_thread([&iocMqtt, &volunteers, &beacon_location] {
            client_type c(iocMqtt);

            pmqtt = &c;

            c.credentials("client-id2", "clientcpp", "server12345")
            .brokers("127.0.0.1", 1883)
            .async_run(boost::asio::detached);

            co_spawn(iocMqtt, publish_data(c, volunteers, beacon_location), boost::asio::detached);

            iocMqtt.run();
        });
        mqtt_thread.detach();

        while (true) {}

    } catch (const std::exception &e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

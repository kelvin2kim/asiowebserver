#include <async_mqtt5.hpp>

#ifndef GLOBALS_H
#define GLOBALS_H

using client_type = async_mqtt5::mqtt_client<boost::asio::ip::tcp::socket>;
extern client_type* pmqtt;

#endif
//
// Copyright (c) 2023-2024 Ivica Siladic, Bruno Iljazovic, Korina Simicevic
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//[hello_world_over_tls
#include <iostream>

#include <boost/asio/io_context.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <async_mqtt5.hpp>

// External customization point.
namespace async_mqtt5 {

template <typename StreamBase>
struct tls_handshake_type<boost::asio::ssl::stream<StreamBase>> {
	static constexpr auto client = boost::asio::ssl::stream_base::client;
	static constexpr auto server = boost::asio::ssl::stream_base::server;
};

// This client uses this function to indicate which hostname it is
// attempting to connect to at the start of the handshaking process.
template <typename StreamBase>
void assign_tls_sni(
	const authority_path& ap,
	boost::asio::ssl::context& ctx,
	boost::asio::ssl::stream<StreamBase>& stream
) {
	SSL_set_tlsext_host_name(stream.native_handle(), ap.host.c_str());
}

} // end namespace async_mqtt5

// The certificate file in the PEM format.
constexpr char ca_cert[] =
"-----BEGIN CERTIFICATE-----\n\
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n\
-----END CERTIFICATE-----\n"
;

int main() {
	boost::asio::io_context ioc;

	// Context satisfying ``__TlsContext__`` requirements that the underlying SSL stream will use.
	// The purpose of the context is to allow us to set up TLS/SSL-related options. 
	// See ``__SSL__`` for more information and options.
	boost::asio::ssl::context context(boost::asio::ssl::context::tls_client);

	async_mqtt5::error_code ec;

	// Add the trusted certificate authority for performing verification.
	context.add_certificate_authority(boost::asio::buffer(ca_cert), ec);
	if (ec)
		std::cout << "Failed to add certificate authority!" << std::endl;
	ec.clear();

	// Set peer verification mode used by the context.
	// This will verify that the server's certificate is valid and signed by a trusted certificate authority.
	context.set_verify_mode(boost::asio::ssl::verify_peer, ec);
	if (ec)
		std::cout << "Failed to set peer verification mode!" << std::endl;
	ec.clear();

	// Construct the Client with ``__SSL_STREAM__`` as the underlying stream
	// with ``__SSL_CONTEXT__`` as the ``__TlsContext__`` type.
	async_mqtt5::mqtt_client<
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket>,
		boost::asio::ssl::context
	> client(ioc, std::move(context));

	// 8883 is the default TLS MQTT port.
	client.brokers("c1fe1ad1.ala.us-east-1.emqxsl.com", 8883)
		.async_run(boost::asio::detached);

	client.async_publish<async_mqtt5::qos_e::at_most_once>(
		"uplink", "Hello world!",
		async_mqtt5::retain_e::no, async_mqtt5::publish_props{},
		[&client](async_mqtt5::error_code ec) {
			std::cout << ec.message() << std::endl;
			client.async_disconnect(boost::asio::detached);
		}
	);

	ioc.run();
}
//]
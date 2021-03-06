#include <unistd.h>

#include <iostream>

#include "../http/http_callbacks.h"
#include "../http/http_client.h"
#include "../http/http_request.h"
#include "../http/http_response.h"
#include "../base/end_point.h"
#include "../base/ip_address.h"

class HttpClientHandler final {
 public:
  HttpClientHandler() = default;
  ~HttpClientHandler() = default;

  using HttpConnectionPtr = std::shared_ptr<cnetpp::http::HttpConnection>;
  bool OnConnected(HttpConnectionPtr http_connection) {
    std::cout << "Connected to the server" << std::endl;
    std::shared_ptr<cnetpp::http::HttpPacket> http_request(
        new cnetpp::http::HttpRequest);
    static_cast<cnetpp::http::HttpRequest*>(http_request.get())->set_method(
        cnetpp::http::HttpRequest::MethodType::kGet);
    static_cast<cnetpp::http::HttpRequest*>(http_request.get())->set_uri("/");
    static_cast<cnetpp::http::HttpRequest*>(http_request.get())->SetHttpHeader(
        "Content-Length", "0");
    return http_connection->SendPacket(http_request->ToString());
  }
  bool OnClosed(HttpConnectionPtr http_connection) {
    std::cout << "Connection closed" << std::endl;
    (void) http_connection;
    return true;
  }
  bool OnReceived(HttpConnectionPtr http_connection) {
    auto http_packet = http_connection->http_packet();
    auto http_response =
        static_cast<cnetpp::http::HttpResponse*>(http_packet.get());
    std::cout << "status: "
              << static_cast<int>(http_response->status()) << std::endl;
    std::cout << "content-length: "
              << http_response->GetHttpHeader("Content-Length") << std::endl;
    if (std::strtol(http_response->GetHttpHeader("Content-Length").c_str(),
                    nullptr,
                    10) > 0) {
      std::cout << "body: " << http_response->http_body() << std::endl;
    }
    http_connection->MarkAsClosed();
    return true;
  }
  bool OnSent(bool success, HttpConnectionPtr http_connection) {
    (void) http_connection;
    std::cout << "send packet successfully" << std::endl;
    return success;
  }
};

int main() {
  using HttpConnectionPtr = std::shared_ptr<cnetpp::http::HttpConnection>;
  HttpClientHandler http_client_handler;
  cnetpp::http::HttpClient http_client;
  cnetpp::http::HttpOptions http_options;
  http_options.set_connected_callback(
      [&http_client_handler] (HttpConnectionPtr c) -> bool {
        return http_client_handler.OnConnected(c);
      }
  );
  http_options.set_closed_callback(
      [&http_client_handler] (HttpConnectionPtr c) -> bool {
        return http_client_handler.OnClosed(c);
      }
  );
  http_options.set_received_callback(
      [&http_client_handler] (HttpConnectionPtr c) -> bool {
        return http_client_handler.OnReceived(c);
      }
  );
  http_options.set_sent_callback(
      [&http_client_handler] (bool success, HttpConnectionPtr c) -> bool {
        return http_client_handler.OnSent(success, c);
      }
  );
  cnetpp::base::IPAddress remote_ip("127.0.0.1");
  cnetpp::base::EndPoint remote_end_point(remote_ip, 12346);
  if (!http_client.Launch(1)) {
    std::cout << "failed to launch the http_client" << std::endl;
    return 1;
  }
  auto connection_id = http_client.Connect(&remote_end_point, http_options);
  if (connection_id < 0) {
    std::cout << "failed to connect to the server" << std::endl;
    return 1;
  }

  sleep(10);

  http_client.Shutdown();

  return 0;
}


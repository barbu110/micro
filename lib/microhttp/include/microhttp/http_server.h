//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#pragma once

#include "microhttp/http_request.h"
#include "microloop/buffer.h"
#include "microloop/net/tcp_server.h"

#include <cstdint>
#include <functional>
#include <map>

namespace microhttp::http
{

struct HttpServerConfig
{
  std::uint16_t port;
  std::uint64_t max_request_size;
};

class HttpServer
{
  struct Client
  {
    PeerConnection &peer_connection;
    microloop::Buffer received_data;

    /**
     * Drop the client.
     */
    void drop(ResponseWriter &);
  };

public:
  using RequestCallback = std::function<void(const HttpRequest &, ResponseWriter &)>;

  /**
   * Create an HTTP server that listens on 0.0.0.0/32:<port>.
   * @param port The port to bind the server to
   */
  HttpServer(std::uint16_t port);

  /**
   * Create an HTTP server based on the given configuration.
   * @param config Custom configuration
   */
  HttpServer(const HttpServerConfig &config);

  /**
   * Set the callback to be called on HTTP requests.
   * @param func The callable to be called
   * @param args The arguments to be bound to the callable
   */
  template <class Func, class... Args>
  void set_request_callback(Func &&func, Args &&... args)
  {
    using namespace std::placeholders;

    auto bound = std::bind(std::forward<Func>(func), std::forward<Args>(args)..., _1, _2);
    on_request = std::move(bound);
  }

private:
  /**
   * Create a default configuration for spawning an HTTP server.
   * @param  port The port to listen on
   * @return The default configuration for an HTTP server
   */
  static HttpServerConfig create_default_config(std::uint16_t port);

  /**
   * Handle incoming TCP connections on the bound port.
   * @param conn The remote connection
   */
  void on_tcp_conn(microloop::net::TcpServer::PeerConnection &);

  /**
   * Handle incoming TCP data. This translates raw TCP connections into HTTP requests.
   * @param conn The remote TCP connection
   * @param buf The received data buffer
   */
  void on_tcp_data(microhttp::net::TcpServer::PeerConnection &, const microloop::Buffer &);

private:
  /**
   * The underlying TCP server to use.
   */
  microloop::TcpServer tcp_server;

  /**
   * Various parameters of the HTTP server.
   */
  HttpServerConfig config;

  /**
   * The callback to be executed when an HTTP request is received.
   */
  RequestCallback on_request;

  /**
   * A map that holds HTTP clients while their request is being processed. The lifetime of an item
   * in this map is considered to be very short.
   *
   * The collection is indexed by the file descriptor of the underlying TCP connection.
   */
  std::map<std::uint32_t, Client> clients;
};

}  // namespace microhttp::http

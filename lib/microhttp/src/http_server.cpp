//
// Copyright (c) 2019 by Victor Barbu. All Rights Reserved.
//

#include "microhttp/http_server.h"

namespace microhttp::http
{

void HttpServer::Client::drop(ResponseWriter &)
{
  peer_connection->close();
}

HttpServer::HttpServer(std::uint16_t port) : tcp_server{port}, config{create_default_config(port)}
{}

HttpServer::HttpServer(const HttpServerConfig &config) : tcp_server{config.port}, config{config}
{}

HttpServerConfig HttpServer::create_default_config(std::uint16_t port)
{
  static const std::uint64_t default_max_request_size = 8192;

  return {.port = port, .max_request_size = default_max_request_size};
}

void HttpServer::on_tcp_conn(microloop::net::TcpServer::PeerConnection &conn)
{
  clients.emplace(conn.fd, Client{.peer_connection = &conn});
}

void HttpServer::on_tcp_data(
    microloop::net::TcpServer::PeerConnection &conn, const microloop::Buffer &buf)
{
  Client &client __attribute__((unused)) = clients[conn.fd];
}

}  // namespace microhttp::http

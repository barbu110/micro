# microloop

[![Build Status](https://travis-ci.com/barbu110/microloop.svg?branch=master)](https://travis-ci.com/barbu110/microloop)

Microloop is a minimalistic event-loop written in C++17 that aims to support
a Linux runtime. Its main task is to support the creation and usage of a TCP
server with modern C++ code. 

A TCP server can be used to implement the HTTP protocol later, for example.
With the current interface it can be created as easy as this:

```cpp
void on_conn(net::TcpServer::PeerConnection &conn)
{
  std::cout << "Received connection\n";
}

void on_data(net::TcpServer::PeerConnection &conn, const microloop::Buffer &buf)
{
  if (buf.empty())
  {
    conn.close();
    return;
  }

  std::cout << "Received: " << static_cast<char *>(buf.data()) << "\n";

  conn.send("goodbye.\n");
}

int main()
{
  microloop::net::TcpServer tcp_server{/* port */};
  tcp_server.set_connection_callback(on_conn);
  tcp_server.set_data_callback(on_data);

  while (true)
  {
    MICROLOOP_TICK();
  }
}
```

## Building the sources

`microloop` uses the CMake build system so the procedure is pretty 
straightforward:

```bash
$ mkdir build; cd build
$ cmake ..
$ make
```

## Testing the created TCP server

After creating the TCP server as described above and building the
source code, we can test it using a simple utility like `ncat`:

```bash
$ printf "hello" | ncat 127.0.0.1 <port>
goodbye.
```

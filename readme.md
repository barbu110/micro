# micro

`micro` is a network-programming framework written with modern C++17. The application is
made out of the following components:

 - [`microloop`][1], which is the low-level component creating the event-loop and a basic
   implementation of a TCP server,
 - [`microhttp`][2], which allows users to easily create an HTTP server, and
 - [`utils`][3], which is a utility library with various components.

The project uses Google's [Abseil][4] for some of its utility components, and [Bazel][5]
as build system. Google Test comes in play when writing unit tests.

The framework can (currently) only be compiled on Linux.

 [1]: https://github.com/barbu110/micro/tree/master/lib/microloop
 [2]: https://github.com/barbu110/micro/tree/master/lib/microloop
 [3]: https://github.com/barbu110/micro/tree/master/lib/utils
 [4]: https://abseil.io
 [5]: https://bazel.build

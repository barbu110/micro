cc_test(
  name = "http_request",
  timeout = "short",
  srcs = ["http_request_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//lib/microhttp:microhttp",
  ],
)

cc_test(
  name = "rfc7230",
  timeout = "short",
  srcs = ["rfc7230_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//lib/microhttp:microhttp",
  ],
)

cc_test(
  name = "request_parser",
  timeout = "short",
  srcs = ["request_parser_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//lib/microhttp:microhttp",
  ],
)

cc_test(
  name = "uri",
  timeout = "short",
  srcs = ["uri_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//lib/microhttp:microhttp",
  ],
)

cc_test(
  name = "version",
  timeout = "short",
  srcs = ["version_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//lib/microhttp:microhttp",
  ],
)

test_suite(name = "full")

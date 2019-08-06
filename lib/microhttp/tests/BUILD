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
  name = "request_parser",
  timeout = "short",
  srcs = ["request_parser_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    "//lib/microhttp:microhttp",
  ],
)

test_suite(name = "full")

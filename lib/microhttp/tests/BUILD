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

# cc_test(
#   name = "request_parser_internal",
#   timeout = "short",
#   srcs = ["request_parser_internal_test.cpp"],
#   deps = [
#     "@gtest//:gtest",
#     "@gtest//:gtest_main",
#     "//lib/microhttp:microhttp",
#   ],
# )

cc_test(
  name = "request_parser",
  timeout = "short",
  srcs = ["request_parser_test.cpp", "request_parser_internal_test.cpp"],
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

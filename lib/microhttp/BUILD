cc_library(
  name = "microhttp",
  srcs = glob(["src/**/*.cpp"]),
  hdrs = glob(["include/**/*.h"]),
  includes = [
    "include",
  ],
  visibility = ["//visibility:public"],
  deps = [
    "//lib/microloop:microloop",
  ],
)

cc_test(
  name = "http_request_test",
  srcs = ["tests/http_request_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    ":microhttp",
  ],
)

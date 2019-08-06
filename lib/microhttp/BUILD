cc_library(
  name = "main",
  srcs = glob(["src/**/*.cpp"]),
  hdrs = glob(["include/**/*.h"]),
  includes = [
    "include",
  ],
  visibility = ["//visibility:public"],
  deps = [
    "//lib/microloop:main",
  ],
)

cc_test(
  name = "http_request_test",
  srcs = ["tests/http_request_test.cpp"],
  deps = [
    "@gtest//:gtest",
    "@gtest//:gtest_main",
    ":main",
  ],
)

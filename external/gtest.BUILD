cc_library(
  name = "main",
  srcs = glob(
    ["src/*.cc"],
    exclude = ["src/gtest-all.cc"]
  ),
  hdrs = glob([
    "include/**/*.h",
    "src/*.h"
  ]),
  includes = ["external/gtest/include"],
  linkopts = ["-pthread"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "main",
  srcs = glob(["src/**/*.cpp", "include/**/*.h"]),
  hdrs = glob(["include/**/*.h"]),
  linkopts = ["-pthread"],
  includes = [
    "include",
  ],
  visibility = ["//visibility:public"],
)

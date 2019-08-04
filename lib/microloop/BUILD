cc_library(
  name = "main",
  srcs = glob(["src/**/*.cpp", "include/**/*.h"]),
  hdrs = glob(["include/**/*.h"]),
  linkopts = ["-pthread"],
  includes = [
    "include",
  ],
  include_prefix = "microloop",
  visibility = ["//apps/buffer:__pkg__"],
)

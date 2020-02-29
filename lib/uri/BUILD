cc_library(
  name = "uri",
  srcs = glob(["src/**/*.cpp", "include/**/*.h"]),
  hdrs = glob(["include/**/*.h"]),
  includes = [
    "include",
  ],
  visibility = ["//visibility:public"],
  deps = [
    "//external/uriparser:uriparser",
    "@com_google_absl//absl/strings",
  ],
)

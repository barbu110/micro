cc_library(
    name = "microhttp",
    srcs = glob(["src/**/*.cpp"]),
    hdrs = glob(["include/**/*.h"]),
    defines = [
        "BUILD_TESTS",  # Use select() function for defines.
    ],
    includes = [
        "include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//lib/microloop",
        "@gtest//:gtest_prod",
    ],
)

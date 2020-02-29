load("@rules_foreign_cc//tools/build_defs:cmake.bzl", "cmake_external")

cmake_external(
  name = "uriparser",
  lib_source = "@uriparser//:all",
  cache_entries = {
    "URIPARSER_BUILD_DOCS": "NO",
    "BUILD_SHARED_LIBS": "NO",
    "URIPARSER_BUILD_TOOLS": "NO",
  },
  static_libraries = ["liburiparser.a"],
  visibility = ["//lib/uri:__pkg__"],
)

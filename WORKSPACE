workspace(name = "micro")

load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
  name = "rules_cc",
  sha256 = "67412176974bfce3f4cf8bdaff39784a72ed709fc58def599d1f68710b58d68b",
  strip_prefix = "rules_cc-b7fe9697c0c76ab2fd431a891dbb9a6a32ed7c3e",
  urls = [
    "https://mirror.bazel.build/github.com/bazelbuild/rules_cc/archive/b7fe9697c0c76ab2fd431a891dbb9a6a32ed7c3e.zip",
    "https://github.com/bazelbuild/rules_cc/archive/b7fe9697c0c76ab2fd431a891dbb9a6a32ed7c3e.zip",
  ],
)

git_repository(
  name = "gtest",
  remote = "https://github.com/google/googletest",
  commit = "105579a6e43908bc88a289d309492eda8be896be",
  shallow_since = "1541619438 -0500",
)

git_repository(
  name = "com_google_absl",
  remote = "https://github.com/abseil/abseil-cpp.git",
  commit = "ac78ffc3bc0a8b295cab9a03817760fd460df2a1",
  shallow_since = "1568303870 -0400",
)

http_archive(
   name = "rules_foreign_cc",
   strip_prefix = "rules_foreign_cc-master",
   url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")
rules_foreign_cc_dependencies(register_default_tools = True)

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

new_git_repository(
  name = "uriparser",
  remote = "https://github.com/uriparser/uriparser.git",
  commit = "737e95f67bc2e5d8b90a1392797b353b52e5124a",
  shallow_since = "1556453065 +0200",
  build_file_content = all_content,
)

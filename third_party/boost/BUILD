# Build Boost library.
# https://github.com/boostorg/boost

package(default_visibility = ["//visibility:public"])

licenses(["notice"])  # Boost Software License

cc_library(
    name = "boost",
    srcs = glob([
        "boost/*/**/*.h",
        "boost/*/**/*.hpp",
    ]),
    includes = ["."],
)

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "hashmap_manager_example",
    srcs = ["hashmap_manager_example.cc"],
    deps = [
        "//tensorflow_serving/core:aspired_versions_manager",
        "//tensorflow_serving/core:availability_preserving_policy",
        "//tensorflow_serving/servables/hashmap:hashmap_source_adapter",
        # "//tensorflow_serving/sources/storage_path:file_system_storage_path_source",
    ],
    copts = [
        '-O0',
        '-g3',
    ]
)

# https://docs.bazel.build/versions/master/be/c-cpp.html#cc_binary
cc_binary(
    name = "hashmap_server_example",
    srcs = ["hashmap_server_example.cc"],
    copts = [
        '-O0',
        '-g3',
    ],
    deps = [
        "//tensorflow_serving/model_servers:server_core",
        "//tensorflow_serving/servables/hashmap:hashmap_source_adapter",
        "//tensorflow_serving/core:availability_preserving_policy",
        "//tensorflow_serving/model_servers:http_server",
    ],
)
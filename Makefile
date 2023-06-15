.DEFAULT_GOAL := manager-example

BAZEL_ARGS = --color=yes --curses=yes --verbose_failures --output_filter=DONT_MATCH_ANYTHING 

manager-example:
	@bazel build ${BAZEL_ARGS} my_servable/hashmap_manager_example
	@echo "" && echo "Program output:\n"
	@TF_ENABLE_ONEDNN_OPTS=0 ./bazel-bin/my_servable/hashmap_manager_example

tf-serving:
	bazel build ${BAZEL_ARGS} tensorflow_serving/model_servers:tensorflow_model_server

update-compdb:
	bazel run @hedron_compile_commands//:refresh_all

dep-graph:
	bazel query --experimental_repo_remote_exec --notool_deps --noimplicit_deps "deps(//my_servable:my_server)" --output graph
./bazel-bin/tensorflow_serving/model_servers/tensorflow_model_server --model_config_file=./models.config --rest_api_port=8501

curl -d '{"instances": [1.0, 2.0, 5.0]}' -X POST 127.0.0.1:8501/v1/models/half_plus_two:predict
#include <chrono>
#include <thread>

#include "tensorflow_serving/config/monitoring_config.pb.h"
#include "tensorflow_serving/core/availability_preserving_policy.h"
#include "tensorflow_serving/model_servers/http_server.h"
#include "tensorflow_serving/model_servers/server_core.h"
#include "tensorflow_serving/servables/hashmap/hashmap_source_adapter.h"

using namespace tensorflow::serving;
// This macro is executed during initialization of static variables
// For some reason it does not get executed when in hashmap_source_adapter.cc
REGISTER_STORAGE_PATH_SOURCE_ADAPTER(HashmapSourceAdapter,
                                     HashmapSourceAdapterConfig);
int main(void) {
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  ServerCore::Options options;
  ModelServerConfig model_server_config;
  auto *model_config =
      model_server_config.mutable_model_config_list()->add_config();
  model_config->set_name("hashmap");
  // This uses the default loader which searches a filesystem directory with the
  // structure <base_path>/<version_number>/<files>

  // There must be some kind of convention for file names which are then loaded
  // by the custom source adapter
  model_config->set_base_path("/workspaces/serving/test_map");
  model_config->set_model_platform("hashmap");

  // Alternatively, one can supply a custom model loader together with an Any
  // proto buffer to load the model in any other way (other structure, RPC call
  // whatever)

  // options.custom_model_config_loader = ...;
  // auto *custom_model_config =
  // model_server_config.mutable_custom_model_config();
  // custom_model_config->mutable_type_url()->assign(
  //     "type.googleapis.com/_packagename_._messagename_");
  // custom_model_config->mutable_value()->assign(...);
  options.model_server_config = model_server_config;

  ::google::protobuf::Any source_adapter_config;
  HashmapSourceAdapterConfig hashmap_source_adapter_config;
  hashmap_source_adapter_config.set_format(
      HashmapSourceAdapterConfig::SIMPLE_CSV);
  source_adapter_config.PackFrom(hashmap_source_adapter_config);
  (*(*options.platform_config_map.mutable_platform_configs())["hashmap"]
        .mutable_source_adapter_config()) = source_adapter_config;

  std::unique_ptr<ServerCore> core;
  options.aspired_version_policy.reset(new AvailabilityPreservingPolicy());
  TF_CHECK_OK(ServerCore::Create(std::move(options), &core));

  // This does not work for custom servable because the ServableHandle type
  // SavedModelBundle is hardcoded into the http server
  // see (see tensorflow_serving/model_servers/http_rest_api_handler.cc line
  // 233)
  //   std::unique_ptr<net_http::HTTPServerInterface> http_server;
  //   MonitoringConfig monitoring_config;
  //   monitoring_config.mutable_prometheus_config()->set_enable(false);
  //   http_server =
  //       CreateAndStartHttpServer(8501, 4, 30000, monitoring_config,
  //       core.get());

  //   http_server->WaitForTermination();

  // One must use a custom server frontend (like http or gRPC) that queries the
  // correct servable type as shown below
  ServableHandle<std::unordered_map<std::string, std::string>> myServable;
  ModelSpec model_spec;
  model_spec.mutable_name()->assign("hashmap");
  auto handle_request = ServableRequest::Latest("hashmap");
  tensorflow::Status handleStatus =
      core->GetServableHandle(model_spec, &myServable);

  if (!handleStatus.ok()) {
    std::cout << handleStatus;
    return 1;
  }

  for (auto val : *myServable) {
    std::cout << val.first << " ; " << val.second << "\n";
  }

  return 0;
}

#include <iostream>
#include <memory>
#include <unordered_map>

// #include "tensorflow_serving/config/file_system_storage_path_source.pb.h"
#include "tensorflow_serving/core/aspired_versions_manager.h"
#include "tensorflow_serving/core/availability_preserving_policy.h"
#include "tensorflow_serving/core/manager.h"
#include "tensorflow_serving/servables/hashmap/hashmap_source_adapter.h"
// #include "tensorflow_serving/sources/storage_path/file_system_storage_path_source.h"

int main(void) {
  using namespace tensorflow;
  std::unique_ptr<serving::AspiredVersionsManager> manager;
  serving::AspiredVersionsManager::Options manager_options;
  manager_options.aspired_version_policy =
      std::unique_ptr<serving::AspiredVersionPolicy>(
          new serving::AvailabilityPreservingPolicy);
  const Status status = serving::AspiredVersionsManager::Create(
      std::move(manager_options), &manager);

  if (!status.ok()) {
    std::cout << status;
    return 1;
  }

  serving::HashmapSourceAdapterConfig hashmap_config;
  hashmap_config.set_format(serving::HashmapSourceAdapterConfig::SIMPLE_CSV);
  std::unique_ptr<serving::HashmapSourceAdapter> hashmap_adapter(
      new serving::HashmapSourceAdapter(hashmap_config));
  ConnectSourceToTarget(hashmap_adapter.get(), manager.get());

  // This triggers the AspiredVersionsCallback
  hashmap_adapter->SetAspiredVersions(
      "default", {serving::ServableData<serving::StoragePath>(
                     {"default", 1}, "./test_map/1")});

  // std::unique_ptr<serving::FileSystemStoragePathSource> path_source;
  // serving::FileSystemStoragePathSourceConfig config;
  // auto *servable = config.add_servables();
  // servable->set_servable_name("default");
  // servable->set_base_path("./test_map");
  // config.set_file_system_poll_wait_seconds(1);
  // TF_CHECK_OK(
  //     serving::FileSystemStoragePathSource::Create(config, &path_source));
  // ConnectSourceToTarget(path_source.get(), hashmap_adapter.get());

  auto handle_request = serving::ServableRequest::Latest("default");
  // The servable must be of the same type as defined in the servable source
  // adapter so in this case an unordered map of strings
  serving::ServableHandle<std::unordered_map<string, string>> myServable;
  Status servableStatus =
      manager->GetServableHandle(handle_request, &myServable);

  // The AspiredVersionsCallback is executed in a separate thread so it takes a
  // while until the servable is available
  while (!servableStatus.ok()) {
    servableStatus = manager->GetServableHandle(handle_request, &myServable);
  }

  if (!servableStatus.ok()) {
    LOG(INFO) << "Zero versions of 'default' servable have been loaded so far";
    std::cout << servableStatus << "\n";
    return 1;
  }

  if (myServable->empty())
    return 1;

  for (auto val : *myServable) {
    std::cout << val.first << " ; " << val.second << "\n";
  }
  return 0;
}
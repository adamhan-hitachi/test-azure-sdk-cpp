#include <azure/core.hpp>
#include <azure/storage/common/storage_exception.hpp>
#include <azure/core/http/curl_transport.hpp>
#include <azure/storage/blobs.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <utility>
#include <curl/curl.h>

using namespace Azure::Storage;
using namespace Azure::Storage::Blobs;

int main() {
 
curl_version_info_data *data = curl_version_info(CURLVERSION_NOW);
std::cout << "libcurl " << data->version << " " << data->version_num << std::endl;
 
  auto azure_credentials =
      std::make_shared<Azure::Storage::StorageSharedKeyCredential>(
          "devstoreaccount1",
          "Eby8vdM02xNOcqFlqUwJPLlmEtlCDXJ1OUzFT50uSRZ6IFsuFq2UVErCz4I6tq/"
          "K1SZFPTOtr/KBHBeksoGMGw==");

  // Override the default HTTP transport adapter.
  BlobClientOptions options;
  options.Transport.Transport =
      std::make_shared<Azure::Core::Http::CurlTransport>();

  // Initialize a new instance of BlobServiceClient
  BlobServiceClient service_client("http://localhost:10000/devstoreaccount1",
                                   azure_credentials, options);
  
  // Re-create container 
  try {
    service_client.DeleteBlobContainer("mycontainer");
  } catch (Azure::Storage::StorageException& e) { 
    std::cout << "Acceptable exception by DeleteBlobContainer() " << e.Message << std::endl;
  }
  
  service_client.CreateBlobContainer("mycontainer");

  std::vector<std::string> objectsName;
  for (int i = 1; i < 50; ++i) {
    objectsName.emplace_back(std::to_string(i));
  }

  std::string objectContent(100, 'c');

  auto container_client = service_client.GetBlobContainerClient("mycontainer");
  for (auto& item : objectsName) {
    auto blob_client = container_client.GetBlockBlobClient(item);
    blob_client.UploadFrom(
        reinterpret_cast<const uint8_t*>(objectContent.c_str()),
        objectContent.size());
  }

  std::cout << "PUT OK" << std::endl;

  auto delete_batch_2 = service_client.CreateBatch();

  // BlobContainerBatch delete_batch = container_client.CreateBatch();
  // DeleteBlobOptions options_2;
  // options_2.DeleteSnapshots = Models::DeleteSnapshotsOption::IncludeSnapshots;

  // Track deferred batch result
  std::vector<
      std::pair<std::string, std::shared_ptr<Azure::Storage::DeferredResponse<
                                 Models::DeleteBlobResult>>>>
      delete_blobs_result;

  //   for (auto& item : objectsName) {
  //     auto delete_result = delete_batch.DeleteBlob(item, options_2);
  //     auto delete_result_ptr = std::make_shared<
  //         Azure::Storage::DeferredResponse<Models::DeleteBlobResult>>(
  //         std::move(delete_result));
  //     delete_blobs_result.emplace_back(
  //         std::make_pair(item, std::move(delete_result_ptr)));
  //   }

  // auto delete_result = delete_batch.DeleteBlob(objectsName[0]);
  auto delete_result_2 = delete_batch_2.DeleteBlob("mycontainer", objectsName[1]);

// Error!!! stand exception buffered  
  try {
    // Azure::Response<Models::SubmitBlobBatchResult> outcome =
    //     container_client.SubmitBatch(delete_batch);
    service_client.SubmitBatch(delete_batch_2);

      std::cout << "SubmitBatch OK" << std::endl;
  } catch (Azure::Storage::StorageException& e) {
    // batch request fail, none of the subrequests were executed
    std::cerr << "SubmitBatch() exception " << e.Message << std::endl;
  }

  //   for (auto& result : delete_blobs_result) {
  //     try {
  //       result.second->GetResponse();
  //       std::cerr << "OK" << result.first << std::endl;
  //     } catch (Azure::Storage::StorageException& e) {
  //       // subrequest fail
  //       std::cerr << "delete error " << result.first << std::endl;

  //     } catch (...) {
  //       std::cerr << "unknown error " << result.first << std::endl;
  //     }
  //   }

  /// Error!!! Buffered exception thrown
  try {
    delete_result_2.GetResponse();
  } catch (Azure::Storage::StorageException& e) {
    // subrequest fail
    std::cerr << "delete error " << std::endl;
  } 

  //   try {
  //     delete_result.GetResponse();
  //   } catch (Azure::Storage::StorageException& e) {
  //     // subrequest fail
  //     std::cerr << "delete error " << std::endl;

  //   } catch (...) {
  //     std::cerr << "unknown error " << std::endl;
  //   }

  return 0;
}

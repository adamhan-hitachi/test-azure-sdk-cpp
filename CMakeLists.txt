cmake_minimum_required(VERSION 3.0.0)
project(azure-sdk-build VERSION 0.1.0)

set(BUILD_TESTING OFF cache)
set(DISABLE_AZURE_CORE_OPENTELEMETRY OFF cache)

# Add this code before creating and linking your application
include(FetchContent)
FetchContent_Declare(
    azuresdk
    GIT_REPOSITORY      https://github.com/Azure/azure-sdk-for-cpp.git
    GIT_TAG             azure-security-attestation_1.1.0
)
FetchContent_GetProperties(azuresdk)
if(NOT azuresdk_POPULATED)
    FetchContent_Populate(azuresdk)

   # file(COPY overlays DESTINATION ${azuresdk_SOURCE_DIR})
   # set(VCPKG_OVERLAY_PORTS ${azuresdk_SOURCE_DIR}/overlays cache)
    # Adding all Azure SDK libraries
    add_subdirectory(${azuresdk_SOURCE_DIR} ${azuresdk_BINARY_DIR} EXCLUDE_FROM_ALL)
    # Adding one Azure SDK Library only (Storage blobs)
    # add_subdirectory(${azuresdk_SOURCE_DIR}/sdk/storage/azure-storage-blobs ${azuresdk_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

add_executable(azure-sdk-build main.cpp)

# After creating the cmake target
target_link_libraries(azure-sdk-build PRIVATE Azure::azure-identity Azure::azure-storage-blobs)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

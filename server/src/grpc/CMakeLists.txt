find_package(Threads REQUIRED)
option(protobuf_MODULE_COMPATIBLE TRUE)
find_package(Protobuf CONFIG REQUIRED)
message(STATUS "Using protobuf ${Protobuf_VERSION}")

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")

# find_package(Boost 1.85 COMPONENTS system thread url filesystem)
find_package(Boost 1.85 REQUIRED)

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_GRPCPP gRPC::grpc++)

file(GLOB SRC_LIST   "*.cc")
foreach(file ${SRC_LIST})
    message("src file : ${file}")
endforeach()

add_library(local_grpc ${SRC_LIST})
target_link_libraries(local_grpc ${_REFLECTION}  ${_GRPC_GRPCPP}  ${_PROTOBUF_LIBPROTOBUF} ${Boost_LIBRARIES})



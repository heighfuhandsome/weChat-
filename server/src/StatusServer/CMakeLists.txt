find_package(Boost 1.85 REQUIRED system thread)

file(GLOB SRC_LIST "*.cpp")

add_executable(statusServer ${SRC_LIST})

target_link_libraries(statusServer local_grpc ${Boost_LIBRARIES} common)
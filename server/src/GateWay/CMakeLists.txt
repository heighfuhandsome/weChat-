find_package(Boost 1.85 REQUIRED COMPONENTS system thread url)

file(GLOB SRC_LIST  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(GateWayServer ${SRC_LIST})
# ${Boost_LIBRARIES}
target_include_directories(GateWayServer PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(GateWayServer jsoncpp common local_grpc ${Boost_LIBRARIES})
find_package(Boost 1.85 REQUIRED)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SRC_LIST)

add_library(common ${SRC_LIST})

target_link_libraries(common mysqlcppconn ${Boost_LIBRARIES} local_grpc jsoncpp hiredis)


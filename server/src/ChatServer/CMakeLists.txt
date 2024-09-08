cmake_minimum_required(VERSION 3.15)

file(GLOB SRC_LIST "*.cpp")

find_package(Boost 1.85 COMPONENTS system thread)

add_executable(ChatServer ${SRC_LIST})

target_link_libraries(ChatServer common ${Boost_LIBRARIES} jsoncpp)
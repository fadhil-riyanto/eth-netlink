cmake_minimum_required(VERSION 4.1.1)
project(linux-netlink VERSION 1.0)

add_compile_options("-O0")
add_compile_options("-ggdb3")
add_compile_options("-g")
add_compile_options("-DLOG_USE_COLOR")

find_package(mnl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(logc 
        ./submodule/log.c/src/log.c
)

add_executable(ethnetlink 
        ./src/main.c
        ./src/netlink.c
        ./src/link.c
)
target_include_directories(ethnetlink PRIVATE ./include)
target_link_libraries(
        ethnetlink logc mnl
)
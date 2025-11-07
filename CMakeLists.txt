cmake_minimum_required(VERSION 4.1.1)
project(linux-netlink VERSION 1.0)

add_compile_options("-O0")
add_compile_options("-ggdb3")
add_compile_options("-g")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_library(src 
#         ./src/connhandler.c
#         ./src/parseopt.c
        
# )

add_executable(ethnetlink ./src/main.c)

target_link_libraries(ethnetlink)
cmake_minimum_required(VERSION 4.1.1)
project(linux-netlink VERSION 1.0)

add_compile_options("-O0")
add_compile_options("-ggdb3")
add_compile_options("-g")
add_compile_options("-DLOG_USE_COLOR")

find_package(mnl)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(submodule 
        ./submodule/log.c/src/log.c
        ./submodule/inih/ini.c
)

add_library(ethnetlink_libs
        ./src/netlink.c
        ./src/link.c
        ./src/print.c
        ./src/utils.c
)
target_include_directories(ethnetlink_libs PRIVATE ./include)

add_executable(ethnetlink 
        ./src/main.c
)
target_include_directories(ethnetlink PRIVATE ./include)

target_link_libraries(ethnetlink
        ethnetlink_libs
        submodule
        mnl
)

# testing 

# TEST: test_pr_func
# goal: create a pr_func that send the log into syslog & stdout
add_executable(test_pr_func
        ./src/print.c
        ./test/pr_func.c
)
target_include_directories(test_pr_func PRIVATE ./include)
add_test(NAME test_pr_func COMMAND ./test_pr_func)


# TEST: test_netnsid
# goal: call a netlink about its device information using ext-mask & netns_id
#       see: https://docs.kernel.org/netlink/specs/rt-link.html#getlink

add_executable(test_netnsid
        ./test/netnsid.c
)
target_include_directories(test_netnsid PRIVATE ./include)
target_link_libraries(test_netnsid ethnetlink_libs mnl)
add_test(NAME test_netnsid COMMAND ./test_netnsid)



# rule make install
install(
        TARGETS ethnetlink
        DESTINATION /usr/bin
)
install(
        FILES ./conf/ethnetlink.ini
        DESTINATION /etc/ethnetlink
)
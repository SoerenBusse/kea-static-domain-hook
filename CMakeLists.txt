cmake_minimum_required(VERSION 3.16)
project(kea_static_domain_hook)
set(CMAKE_CXX_STANDARD 11)

# Explicitly include the kea headers
set(KEA_INCLUDE_PATH "/usr/include/kea")
include_directories(${KEA_INCLUDE_PATH})

add_library(kea_static_domain_hook SHARED src/version.cpp src/library_common.h src/callouts.cpp src/messages.cc src/messages.h)

# Link against kea libraries
target_link_libraries(kea_static_domain_hook -lkea-dhcpsrv -lkea-dhcp++ -lkea-hooks -lkea-log -lkea-util -lkea-exceptions)

# Explicitly enable PIC for our target although SHARED should enable it by default
set_property(TARGET kea_static_domain_hook PROPERTY POSITION_INDEPENDENT_CODE ON)
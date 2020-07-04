cmake_minimum_required(VERSION 3.0)

project(elemlist)

add_executable(elemlist main.cpp)

set_property(TARGET elemlist PROPERTY CXX_STANDARD 17)
set_property(TARGET elemlist PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE RelWithDebInfo)
set(CMAKE_BUILD_TYPE Debug)


find_package(Curses REQUIRED)

target_link_libraries(elemlist ${CURSES_LIBRARIES} -lform -ltinfo -lncursesw)

install(TARGETS elemlist RUNTIME DESTINATION bin)

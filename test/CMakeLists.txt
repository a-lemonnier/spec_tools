cmake_minimum_required(VERSION 2.6)
project(spec_tools)

set(TEST_FILES ./test/test_marker.cpp)

find_package( Boost 1.40 COMPONENTS program_options 
                         REQUIRED timer filesystem unit_test_framework)

include_directories(./src ./include ${Boost_INCLUDE_DIRS})
                         
add_definitions (-DBOOST_TEST_DYN_LINK)

add_executable(test_all ${TEST_FILES})

set_property(TARGET test_all PROPERTY CXX_STANDARD 17)
set_property(TARGET test_all PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(test_all ${Boost_LIBRARIES})

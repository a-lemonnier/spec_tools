cmake_minimum_required(VERSION 2.6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ".")
set(PROJECT_UNIT_TESTS_NAME ${PROJECT_NAME_STR}_unit_tests)


set(TEST_FILES ./test/test_marker.cpp)
set(SRC_FILES ./src)
set(INC_FILES ./include)

find_package(Boost COMPONENTS filesystem system unit_test_framework REQUIRED)


include_directories(../src ../include ${Boost_INCLUDE_DIRS})
                         
add_definitions (-DBOOST_TEST_DYN_LINK)

add_executable(${PROJECT_UNIT_TESTS_NAME} ${SRC_FILES} ${INC_FILES} ${TEST_FILES} ${Boost_INCLUDE_DIRS})

target_link_libraries(${PROJECT_UNIT_TESTS_NAME} ${TEST_FILES} ${Boost_LIBRARIES})

enable_testing()
add_test(Tests COMMAND ${PROJECT_UNIT_TESTS_NAME})

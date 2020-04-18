cmake_minimum_required(VERSION 2.6)

find_package( Boost 1.40 REQUIRED unit_test_framework)

include_directories (${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/include
                     ${Boost_INCLUDE_DIRS} )

add_definitions (-DBOOST_TEST_DYN_LINK)

add_executable (Tests test_marker.cpp) 
 
target_link_libraries(tests
  toto
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )


# enable_testing()
# 
# set (Boost_USE_STATIC_LIBS OFF)
# 
# set(SRC_FILES src/*.cpp include/*.cpp)
# 
# target_link_libraries(unit_tests
#     ${SRC_FILES}
#     ${Boost_LIBRARIES}
#     Boost::unit_test_framework
# )
# 
# target_compile_definitions(unit_tests
#     PRIVATE
#         BOOST_TEST_DYN_LINK
# )
# 
# add_test(test_all unit_tests)




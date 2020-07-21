cmake_minimum_required(VERSION 3.0)

project(spec_tools)

set(COMPILER gcc)

if (COMPILER STREQUAL "gcc")
  set(CMAKE_CC_COMPILER "gcc")
  set(CMAKE_CXX_COMPILER "g++")
  message("CC: gcc")
  message("CXX: g++")
elseif (COMPILER STREQUAL "clang")
  set(CMAKE_CC_COMPILER "clang")
  set(CMAKE_CXX_COMPILER "clang++")
  message("CC: clang")
  message("CXX: clang++")
elseif (COMPILER STREQUAL "intel") #does not work atm
  set(INTEL_DIR  /opt/intel/sw_dev_tools/bin)
  set(CMAKE_CC_COMPILER ${INTEL_DIR}/icc)
  set(CMAKE_CXX_COMPILER ${INTEL_DIR}/icpc)
  message("CC: icc")
  message("CXX: icpc")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_executable(threshold src/threshold.cpp include/msg.cpp include/log.cpp)
add_executable(findncopy src/findncopy.cpp include/msg.cpp include/log.cpp)
add_executable(trim src/trim.cpp include/msg.cpp include/log.cpp)
add_executable(der_snr src/der_snr.cpp include/msg.cpp include/log.cpp)
add_executable(shift src/shift.cpp include/msg.cpp include/log.cpp)
add_executable(genrandspec src/genrandspec.cpp include/msg.cpp include/log.cpp)
add_executable(marker src/marker.cpp include/msg.cpp include/log.cpp)
add_executable(elemlist src/elemlist.cpp include/msg.cpp include/log.cpp)
add_executable(waverage src/waverage.cpp include/waverage.hpp)

add_library(msg include/msg.cpp include/msg.h include/log.h)
add_library(log include/log.cpp include/log.h)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set_property(TARGET threshold PROPERTY CXX_STANDARD 17)
set_property(TARGET threshold PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET findncopy PROPERTY CXX_STANDARD 17)
set_property(TARGET findncopy PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET trim PROPERTY CXX_STANDARD 17)
set_property(TARGET trim PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET der_snr PROPERTY CXX_STANDARD 17)
set_property(TARGET der_snr PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET shift PROPERTY CXX_STANDARD 17)
set_property(TARGET shift PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET genrandspec PROPERTY CXX_STANDARD 17)
set_property(TARGET genrandspec PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET marker PROPERTY CXX_STANDARD 17)
set_property(TARGET marker PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET elemlist PROPERTY CXX_STANDARD 17)
set_property(TARGET elemlist PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET msg PROPERTY CXX_STANDARD 17)
set_property(TARGET msg PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET log PROPERTY CXX_STANDARD 17)
set_property(TARGET log PROPERTY CXX_STANDARD_REQUIRED ON)

set_property(TARGET waverage PROPERTY CXX_STANDARD 17)
set_property(TARGET waverage PROPERTY CXX_STANDARD_REQUIRED ON)

find_package( Boost 1.40 COMPONENTS program_options 
                         REQUIRED timer filesystem unit_test_framework)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package (GTK2 QUIET)

find_package(PkgConfig REQUIRED)
pkg_search_module(GLIB QUIET glib-2.0)
pkg_search_module(GTK QUIET 2.0)


include_directories(./src ./include ${Boost_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GTK2_INCLUDE_DIRS} ./test)
link_directories(${GLIB_LIBRARY_DIRS})

target_link_libraries(msg LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(log LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)

target_link_libraries(threshold LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(findncopy LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(trim LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(der_snr LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(shift LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(genrandspec LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(marker LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(elemlist LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)

target_link_libraries(waverage LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs -lCCfits -lcfitsio Eigen3::Eigen -lnotify)

install(TARGETS threshold RUNTIME DESTINATION bin)
install(TARGETS findncopy RUNTIME DESTINATION bin)
install(TARGETS trim RUNTIME DESTINATION bin)
install(TARGETS der_snr RUNTIME DESTINATION bin)
install(TARGETS shift RUNTIME DESTINATION bin)
install(TARGETS genrandspec RUNTIME DESTINATION bin)
install(TARGETS marker RUNTIME DESTINATION bin)
install(TARGETS waverage RUNTIME DESTINATION bin)

function(create_test exe)
    add_executable (test_${exe} ./test/test_${exe}.cpp) 
    set_property(TARGET test_${exe} PROPERTY CXX_STANDARD 17)
    set_property(TARGET test_${exe} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_include_directories(test_${exe} PRIVATE ${BOOST_INCLUDE_DIRS})
    target_link_libraries(test_${exe} msg ${Boost_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} -Wno-c++17-extensions)
    add_test (NAME test_${exe} COMMAND test_${exe} )
endfunction()


enable_testing ()

include_directories (${CMAKE_SOURCE_DIR}/src
                     ${CMAKE_SOURCE_DIR}/include
                     ${Boost_INCLUDE_DIRS} )

add_definitions (-DBOOST_TEST_DYN_LINK)

create_test(marker)
create_test(csv)
create_test(msg)


cmake_minimum_required(VERSION 3.0)

project(spec_tools)

add_subdirectory(./include)

add_executable(threshold threshold.cpp include/msg.cpp)
add_executable(findncopy findncopy.cpp include/msg.cpp)
add_executable(trim trim.cpp include/msg.cpp)
add_executable(der_snr der_snr.cpp include/msg.cpp)
add_executable(shift shift.cpp include/msg.cpp)
add_executable(genrandspec genrandspec.cpp include/msg.cpp)
add_executable(marker marker.cpp include/msg.cpp)

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

find_package( Boost 1.40 COMPONENTS program_options REQUIRED timer filesystem)
include_directories( ${Boost_INCLUDE_DIR})

target_link_libraries(threshold LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(findncopy LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(trim LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(der_snr LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(shift LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(genrandspec LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)
target_link_libraries(marker LINK_PUBLIC ${Boost_LIBRARIES} -lpthread -lstdc++fs)

install(TARGETS threshold RUNTIME DESTINATION bin)
install(TARGETS findncopy RUNTIME DESTINATION bin)
install(TARGETS trim RUNTIME DESTINATION bin)
install(TARGETS der_snr RUNTIME DESTINATION bin)
install(TARGETS shift RUNTIME DESTINATION bin)
install(TARGETS genrandspec RUNTIME DESTINATION bin)
install(TARGETS marker RUNTIME DESTINATION bin)

cmake_minimum_required(VERSION 3.8)
project(cv_test)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp DicomReader.cpp DicomReader.h)
add_executable(cv_test ${SOURCE_FILES})
target_link_libraries(cv_test dcmdata)
target_link_libraries(cv_test dcmimgle)
target_link_libraries(cv_test opencv_core)
target_link_libraries(cv_test opencv_highgui)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic")
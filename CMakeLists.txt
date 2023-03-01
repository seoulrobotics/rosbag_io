cmake_minimum_required(VERSION 3.0.2)
cmake_policy(SET CMP0076 NEW)

project(rosbag_io VERSION 1.0.0)

set_directory_properties(PROPERTIES COMPILE_OPTIONS "-Wall;-Wextra")

find_package(Boost REQUIRED COMPONENTS filesystem system thread)
find_package(BZip2 REQUIRED)

# execinfo.h is needed for backtrace on glibc systems
include(CheckIncludeFile)
include(CheckFunctionExists)
include(CheckCXXSourceCompiles)
CHECK_INCLUDE_FILE(execinfo.h HAVE_EXECINFO_H)
if(HAVE_EXECINFO_H)
  add_definitions(-DHAVE_EXECINFO_H=1)
endif(HAVE_EXECINFO_H)
# do we have demangle capability?
# CHECK_INCLUDE_FILE doesn't work here for some reason
CHECK_CXX_SOURCE_COMPILES("#include<cxxabi.h>\nint main(int argc,char**argv){}" HAVE_CXXABI_H)
if(HAVE_CXXABI_H)
  add_definitions(-DHAVE_CXXABI_H=1)
endif()
CHECK_FUNCTION_EXISTS(backtrace HAVE_GLIBC_BACKTRACE)
if(HAVE_GLIBC_BACKTRACE)
  add_definitions(-DHAVE_GLIBC_BACKTRACE)
endif(HAVE_GLIBC_BACKTRACE)

find_path(lz4_INCLUDE_DIRS NAMES lz4.h)
if (NOT lz4_INCLUDE_DIRS)
  message(FATAL_ERROR "lz4 includes not found")
endif()
find_library(lz4_LIBRARIES NAMES lz4 liblz4)
if (NOT lz4_LIBRARIES)
  message(FATAL_ERROR "lz4 library not found")
endif()
set(lz4_FOUND TRUE)

# Support large bags (>2GB) on 32-bit systems
add_definitions(-D_FILE_OFFSET_BITS=64)

include_directories(include ${Boost_INCLUDE_DIRS} ${BZIP2_INCLUDE_DIR} ${lz4_INCLUDE_DIRS})
add_definitions(${BZIP2_DEFINITIONS})

add_library(${PROJECT_NAME} STATIC)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${BZIP2_LIBRARIES} ${lz4_LIBRARIES})

add_subdirectory(src)

# Install library
include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME} LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Seoul Robotics")
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libbz2-dev, liblz4-dev, libboost-system-dev, libboost-filesystem-dev, libboost-thread-dev")
INCLUDE(CPack)
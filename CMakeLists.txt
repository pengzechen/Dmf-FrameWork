#CMake最低版本号要求
cmake_minimum_required(VERSION 3.10.0)

#指定项目名称
project(DmfServer)

#可以输出中文
if( CMAKE_SYSTEM_NAME STREQUAL "Windows" )
add_definitions("-fexec-charset=gbk")
endif()

#指定版本信息
set(CMAKE_SYSTEM_VERSION 1)

#指定编译目录
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

#指定二进制文件输出目录
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# set (CMAKE_BUILD_TYPE "Debug")

#添加子目录
add_subdirectory(${PROJECT_SOURCE_DIR}/Src) 

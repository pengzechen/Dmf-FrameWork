#CMake最低版本号要求
cmake_minimum_required(VERSION 3.10.0)

#指定项目名称
project(CMakeDemo)

#指定版本信息
set(CMAKE_SYSTEM_VERSION 1)

#指定编译目录
set(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)

#添加子目录,这样进入源码文件src目录可以继续构建  
add_subdirectory(${PROJECT_SOURCE_DIR}/Src) 

# 设置 可执行文件的输出路径为此目录
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/mdb")
# 设置 dll 的输出路径为此目录
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/mdb")


# 添加动态链接库
file( GLOB libmdb_SRC "mdb_operate.c")
add_library(mdb_operate SHARED ${libmdb_SRC} )

# 编译 mdbserver
add_executable(mdbserver "mdbserver.c")


# 编译 mdbrw
link_directories("${PROJECT_SOURCE_DIR}/bin/mdb/")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    link_libraries("mdb_operate")
else(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    link_libraries("libmdb_operate")
endif()

add_executable(rw3 "mdbrw3.c")



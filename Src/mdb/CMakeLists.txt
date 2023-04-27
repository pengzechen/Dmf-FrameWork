
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/mdb)
# 设置 dll 的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/mdb)
# 获得本目录的 文件
file( GLOB mdb_SRC "libshare.c")
# 添加动态链接库
add_library(share SHARED ${mdb_SRC} )


# 指定生成目标
add_executable(sharetest "share.c")

add_executable(mdb3 "mdb3.c")

add_executable(rw3 "mdbrw3.c")
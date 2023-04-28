
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/mdb)
# 设置 dll 的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/mdb)



# file( GLOB mdb_SRC "test.c")
# 添加动态链接库
# add_library(share SHARED ${mdb_SRC} )


add_executable(mdbserver "mdbserver.c")

add_executable(rw3 "mdbrw3.c")
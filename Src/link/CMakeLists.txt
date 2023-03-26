

# 设置 dll 的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/link)
# 获得本目录的 文件
file( GLOB link_SRC "*.c")
# 添加动态链接库
add_library(link SHARED ${link_SRC} )


# 指定生成目标
add_executable(linktest "linktest.c")
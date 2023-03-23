

# 设置 dll 的输出路径
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/views)
# 获得本目录的 view
file( GLOB views_SRC "*.c" "*.h")
# 添加动态链接库
add_library(views SHARED ${views_SRC} ${views_need_SRC})


# 指定生成目标
add_executable(linktest "linktest.c")
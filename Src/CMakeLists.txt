#指定头文件目录,PROJECT_SOURCE_DIR为工程的根目录  
include_directories(${PROJECT_SOURCE_DIR}/include)

#指定可执行文件的输出目录，输出到bin下面  
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

link_directories("${PROJECT_SOURCE_DIR}/lib")

add_definitions("-DHAVE_STRUCT_TIMESPEC")

#指定生成目标
add_executable(server server.c request.c template.c response.c router.c model.c main.c utility.c session.c cpool.c)
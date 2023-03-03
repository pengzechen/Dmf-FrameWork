
include_directories(${PROJECT_SOURCE_DIR}/include)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

link_directories("${PROJECT_SOURCE_DIR}/lib")

add_definitions("-DHAVE_STRUCT_TIMESPEC")

file( GLOB main_SRC "*.h" "*.c" )

aux_source_directory(./model model_SRC)

#指定生成目标
add_executable(server ${main_SRC} ${model_SRC})
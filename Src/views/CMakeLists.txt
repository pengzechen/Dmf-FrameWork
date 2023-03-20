SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/views)
file( GLOB views_SRC "*.c" )
ADD_LIBRARY(views SHARED ${views_SRC} ${views_need_SRC})
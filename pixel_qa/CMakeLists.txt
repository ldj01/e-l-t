include_directories(${XML_INCLUDES})
include_directories(${ESPA_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/level1_lib)

file(GLOB SOURCES "*.c")

add_library(espa_pixel_qa STATIC ${SOURCES})

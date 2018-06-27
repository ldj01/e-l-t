include_directories(${XML_INCLUDES})
include_directories(${ESPA_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/level1_lib)
include_directories(${CMAKE_BINARY_DIR})

file(GLOB SOURCES "*.c")

add_library(espa_pixel_qa STATIC ${SOURCES})

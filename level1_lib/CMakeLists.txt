include_directories(${XML_INCLUDES})
include_directories(${ESPA_INCLUDES})

file(GLOB SOURCES "*.c")

add_library(espa_level1_qa STATIC ${SOURCES})

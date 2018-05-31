include_directories(${XML_INCLUDES})
include_directories($ENV{ESPAINC})

file(GLOB SOURCES "*.c")

add_library(espa_level2_qa STATIC ${SOURCES})

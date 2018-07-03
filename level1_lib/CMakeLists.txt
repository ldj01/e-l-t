include_directories(${XML_INCLUDES})
include_directories(${ESPA_INCLUDES})

file(GLOB SOURCES "*.c")

add_library(espa_level1_qa STATIC ${SOURCES})
set_target_properties(espa_level1_qa PROPERTIES PUBLIC_HEADER read_level1_qa.h)

install(TARGETS espa_level1_qa
        ARCHIVE DESTINATION ${ESPA_LIBS}
        PUBLIC_HEADER DESTINATION ${ESPA_INCLUDES})

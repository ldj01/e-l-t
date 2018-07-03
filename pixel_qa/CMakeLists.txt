include_directories(${XML_INCLUDES})
include_directories(${ESPA_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/level1_lib)
include_directories(${CMAKE_BINARY_DIR})

file(GLOB SOURCES "*.c")
file(GLOB HEADERS "*.h")

add_library(espa_pixel_qa STATIC ${SOURCES})
set_target_properties(espa_pixel_qa PROPERTIES PUBLIC_HEADER "${HEADERS}")

install(TARGETS espa_pixel_qa
        ARCHIVE DESTINATION ${ESPA_LIBS}
        PUBLIC_HEADER DESTINATION ${ESPA_INCLUDES})

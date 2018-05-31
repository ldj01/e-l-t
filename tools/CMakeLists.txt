include_directories(${XML_INCLUDES})
include_directories(${ESPA_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/common)
include_directories(${CMAKE_SOURCE_DIR}/level1_lib)
include_directories(${CMAKE_SOURCE_DIR}/level2_lib)
include_directories(${CMAKE_SOURCE_DIR}/pixel_qa)

link_directories(${ESPA_LIBS})

add_executable(test_read_level1_qa test_read_level1_qa.c)
target_link_libraries(test_read_level1_qa
                      espa_level1_qa espa_raw_binary espa_common
                      ${XML_LIBS})

add_executable(generate_pixel_qa generate_pixel_qa.c)
target_link_libraries(generate_pixel_qa
                      espa_pixel_qa espa_level1_qa espa_raw_binary espa_common
                      ${XML_LIBS})

add_executable(dilate_pixel_qa dilate_pixel_qa.c)
target_link_libraries(dilate_pixel_qa
                      espa_pixel_qa espa_raw_binary espa_common
                      ${XML_LIBS})

add_executable(test_read_pixel_qa test_read_pixel_qa.c)
target_link_libraries(test_read_pixel_qa
                      espa_pixel_qa espa_raw_binary espa_common
                      ${XML_LIBS})

add_executable(test_read_level2_qa test_read_level2_qa.c)
target_link_libraries(test_read_level2_qa
                      espa_level2_qa espa_raw_binary espa_common
                      ${XML_LIBS})

set (ALL_TARGETS test_read_level1_qa generate_pixel_qa dilate_pixel_qa
                 test_read_pixel_qa test_read_level2_qa)

install(TARGETS ${ALL_TARGETS} DESTINATION bin)

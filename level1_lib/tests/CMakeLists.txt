include_directories(${CMAKE_SOURCE_DIR}/level1_lib)
include_directories(${CMAKE_BINARY_DIR})
include_directories(SYSTEM ${ESPA_INCLUDES})
include_directories(SYSTEM ${IAS_INCLUDES})
include_directories(SYSTEM ${XML_INCLUDES})

link_directories(${ESPA_LIBS})

add_executable(test_read_level1_qa test_read_level1_qa.c)
target_link_libraries(test_read_level1_qa
                      espa_level1_qa espa_raw_binary espa_common
                      ${XML_LIBS})

add_test(NAME read_level1_qa COMMAND test_read_level1_qa
         --xml LC08_L1TP_043028_20160401_20181005_02_T1.xml
         WORKING_DIRECTORY $ENV{ESPA_UNIT_TEST_DATA_DIR}/espa-l2qa-tools)


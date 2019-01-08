include_directories(${CMAKE_SOURCE_DIR}/level2_lib)
include_directories(${CMAKE_BINARY_DIR})
include_directories(SYSTEM ${ESPA_INCLUDES})
include_directories(SYSTEM ${IAS_INCLUDES})
include_directories(SYSTEM ${XML_INCLUDES})

link_directories(${ESPA_LIBS})

add_executable(test_read_level2_qa test_read_level2_qa.c)
target_link_libraries(test_read_level2_qa
                      espa_level2_qa espa_raw_binary espa_common
                      ${XML_LIBS})

add_test(NAME read_level2_qa_0 COMMAND test_read_level2_qa
         --xml LE07_L1GT_125107_20021230_20170127_01_T2.xml --qa_type 0
         WORKING_DIRECTORY $ENV{ESPA_UNIT_TEST_DATA_DIR}/espa-l2qa-tools)
add_test(NAME read_level2_qa_1 COMMAND test_read_level2_qa
         --xml LC09_L1TP_017038_20130521_20180502_01_A1.xml --qa_type 1
         WORKING_DIRECTORY $ENV{ESPA_UNIT_TEST_DATA_DIR}/espa-l2qa-tools)


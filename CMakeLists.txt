cmake_minimum_required(VERSION 3.6)

project(espa-l2qa-tools)

# Set the package version.
include(package_version.cmake)

include(FindPkgConfig)
include (CTest)

if (NOT XML_INCLUDES)
    find_package(LibXml2)
    if (LIBXML2_FOUND)
        set (XML_INCLUDES ${LIBXML2_INCLUDE_DIR})
        set (XML_LIBS ${LIBXML2_LIBRARIES})
    endif ()
endif ()
set (XML_INCLUDES ${XML_INCLUDES} CACHE PATH "XML2 header file path" FORCE)
set (XML_LIBS ${XML_LIBS} CACHE STRING "XML2 libraries" FORCE)

# Set the paths for the ESPA headers and libraries.
if (NOT ESPA_INCLUDES)
    if (DEFINED ENV{ESPAINC})
        set (ESPA_INCLUDES $ENV{ESPAINC})
    else ()
        set (ESPA_INCLUDES ${CMAKE_INSTALL_PREFIX}/include)
    endif ()
endif ()
if (NOT ESPA_LIBS)
    if (DEFINED ENV{ESPALIB})
        set (ESPA_LIBS $ENV{ESPALIB})
    else ()
        set (ESPA_LIBS ${CMAKE_INSTALL_PREFIX}/lib)
    endif ()
endif ()
set (ESPA_INCLUDES ${ESPA_INCLUDES} CACHE PATH "ESPA header file path" FORCE)
set (ESPA_LIBS ${ESPA_LIBS} CACHE PATH "ESPA library path" FORCE)

# Set the path for the IAS headers
if (NOT IAS_INCLUDES)
    set (IAS_INCLUDES $ENV{IAS_INC})
endif ()
set (IAS_INCLUDES ${IAS_INCLUDES} CACHE PATH "IAS library header file path"
     FORCE)

# Control application threading.
if (DEFINED ENV{ENABLE_THREADING})
    add_definitions(-fopenmp)
    set (OPENMP_LIBS "-lgomp")
endif ()

configure_file(common/l2qa_common.h.in l2qa_common.h)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/l2qa_common.h
        DESTINATION ${ESPA_INCLUDES})

add_subdirectory(level1_lib)
add_subdirectory(level2_lib)

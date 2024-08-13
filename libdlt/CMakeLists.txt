cmake_minimum_required(VERSION 3.9)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

project(libdlt VERSION 1.0.0 DESCRIPTION "c library for dlt messages")

include(GNUInstallDirs)

add_library(dlt SHARED src/dltparser.c)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    include(CodeCoverage)
    append_coverage_compiler_flags_to_target(dlt)
endif()
if (CMAKE_BUILD_TYPE MATCHES "Release")
    find_package(Doxygen)

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)

    add_custom_target(
    doc ALL
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
    )
endif()

set_target_properties(dlt PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/dlt.h)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc.in ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc @ONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/dlt.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/dlt.h @ONLY)

target_include_directories(dlt PRIVATE include)

install(TARGETS dlt
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# test
include(CTest)


add_executable(dlt_test_context test/test_context.c)
target_include_directories(dlt_test_context PRIVATE include)
target_link_libraries(dlt_test_context dlt)

add_executable(dlt_test test/test.c)
target_include_directories(dlt_test PRIVATE include)
target_link_libraries(dlt_test dlt)

add_test(NAME test_context COMMAND dlt_test_context)
add_test(NAME test_on_testfile COMMAND dlt_test ${CMAKE_CURRENT_SOURCE_DIR}/test/testfile.dlt)
add_test(NAME test_on_testfile_extended COMMAND dlt_test ${CMAKE_CURRENT_SOURCE_DIR}/test/testfile_extended.dlt)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    if(CMAKE_COMPILER_IS_GNUCXX)
        setup_target_for_coverage_gcovr_html(NAME ${PROJECT_NAME}_coverage test_context coverage)
    endif()
endif()

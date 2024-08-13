cmake_minimum_required(VERSION 3.9)

project(libdltparser VERSION 1.0.0 DESCRIPTION "c library for parsing dlt messages")

include(GNUInstallDirs)

add_library(dltparser SHARED src/dltparser.c)

set_target_properties(dltparser PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER include/dltparser.h)

configure_file(libdltparser.pc.in libdltparser.pc @ONLY)

target_include_directories(dltparser PRIVATE include)

install(TARGETS dltparser
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/libdltparser.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

# test
include(CTest)

add_executable(dlt_test test/test.c)
target_include_directories(dlt_test PRIVATE include)
target_link_libraries(dlt_test dltparser)

add_test(NAME test_on_testfile COMMAND dlt_test ${CMAKE_CURRENT_SOURCE_DIR}/test/testfile.dlt)
add_test(NAME test_on_testfile_extended COMMAND dlt_test ${CMAKE_CURRENT_SOURCE_DIR}/test/testfile_extended.dlt)

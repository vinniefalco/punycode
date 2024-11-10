#
# Copyright (c) 2024 Vinnie Falco (vinnie.falco@gmail.com)
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/cppalliance/punycode
#

cmake_minimum_required(VERSION 3.8...3.16)

set(BOOST_PUNYCODE_VERSION 1)
if(BOOST_SUPERPROJECT_VERSION)
    set(BOOST_PUNYCODE_VERSION ${BOOST_SUPERPROJECT_VERSION})
endif()

project(boost_punycode VERSION "${BOOST_PUNYCODE_VERSION}" LANGUAGES CXX)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    include(CTest)
    option(BOOST_PUNYCODE_INSTALL "Install boost::punycode files" ON)
    option(BOOST_PUNYCODE_BUILD_TESTS "Build boost::punycode tests" ${BUILD_TESTING})
    set(BOOST_PUNYCODE_IS_ROOT ON)
else()
    set(BOOST_PUNYCODE_BUILD_TESTS OFF CACHE BOOL "")
    set(BOOST_PUNYCODE_IS_ROOT OFF)
endif()

include(GNUInstallDirs)
if(BOOST_PUNYCODE_IS_ROOT)
    set(BOOST_INCLUDE_LIBRARIES punycode url)
    set(BOOST_EXCLUDE_LIBRARIES punycode)
    set(CMAKE_FOLDER Dependencies)
    add_subdirectory(../.. Dependencies/boost EXCLUDE_FROM_ALL)
    unset(CMAKE_FOLDER)
endif()

function(boost_punycode_setup_properties target)
    target_compile_features(${target} PUBLIC cxx_constexpr)
    target_compile_definitions(${target} PUBLIC BOOST_PUNYCODE_NO_LIB=1)

    if(BOOST_SUPERPROJECT_VERSION)
        target_include_directories(${target}
            PUBLIC
                "${PROJECT_SOURCE_DIR}/include"
            PRIVATE
                "${PROJECT_SOURCE_DIR}/"
            )
    else()
        target_include_directories(${target}
            PUBLIC
                "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
                "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
            PRIVATE
                "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/>"
        )
    endif()

    target_link_libraries(${target}
        PUBLIC
        # VFALCO The unit tests assume these libs are dependencies,
        # but the test CML should really explicitly link to them also
        Boost::assert
        Boost::config
        Boost::core
        Boost::system
        Boost::utility
        Boost::url
    )
endfunction()

file(GLOB_RECURSE BOOST_PUNYCODE_HEADERS CONFIGURE_DEPENDS
    include/boost/*.hpp
    include/boost/*.ipp
    include/boost/*.natvis
)
file(GLOB_RECURSE BOOST_PUNYCODE_SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.hpp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/include/boost PREFIX "" FILES ${BOOST_PUNYCODE_HEADERS})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/src PREFIX "src" FILES ${BOOST_PUNYCODE_SOURCES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "" FILES Jamfile)

add_library(boost_punycode
    ${BOOST_PUNYCODE_HEADERS}
    ${BOOST_PUNYCODE_SOURCES}
    #build/Jamfile
    )

add_library(Boost::punycode ALIAS boost_punycode)
boost_punycode_setup_properties(boost_punycode)

target_compile_definitions(boost_punycode
  PUBLIC
  BOOST_PUNYCODE_NO_LIB
)

if(BUILD_SHARED_LIBS)
    target_compile_definitions(boost_punycode PUBLIC BOOST_PUNYCODE_DYN_LINK=1)
else()
    target_compile_definitions(boost_punycode PUBLIC BOOST_PUNYCODE_STATIC_LINK=1)
endif()
target_compile_definitions(boost_punycode PRIVATE BOOST_PUNYCODE_SOURCE)

if(BOOST_PUNYCODE_INSTALL AND NOT BOOST_SUPERPROJECT_VERSION)
    install(TARGETS boost_punycode
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()

if(BOOST_PUNYCODE_BUILD_TESTS)
    add_subdirectory(test)
endif()

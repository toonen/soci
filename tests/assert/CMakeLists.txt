###############################################################################
#
# This file is part of CMake configuration for SOCI library
#
# Copyright (C) 2010-2013 Mateusz Loskot <mateusz@loskot.net>
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
###############################################################################

include_directories(
  ${SOCI_SOURCE_DIR}/include/private
  ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB SOCI_TESTS_COMMON common-tests.h)

add_subdirectory(empty)
add_subdirectory(db2)
add_subdirectory(firebird)
add_subdirectory(mysql)
add_subdirectory(odbc)
add_subdirectory(oracle)
add_subdirectory(postgresql)
add_subdirectory(sqlite3)

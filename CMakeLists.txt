#############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

IF (COMMAND cmake_policy)
	cmake_policy (SET CMP0003 NEW)
ENDIF (COMMAND cmake_policy)

PROJECT (gtorrent-gtk)

SET (GTORRENT_VERSION_MAJOR 0)
SET (GTORRENT_VERSION_MINOR 0)
SET (GTORRENT_VERSION_PATCH 2)

#############################################################

# Configure version into utils/Version.hpp
SET (VERSION ${GTORRENT_VERSION_MAJOR}.${GTORRENT_VERSION_MINOR}.${GTORRENT_VERSION_PATCH})
CONFIGURE_FILE (src/utils/Version.hpp.in Version.hpp @ONLY)

# Set Cmake to build runtime in cwd
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# Options
OPTION (HAVE_WINDEBUG "Enable -mwindows for debugging purposes" ON)

# Windows debug
IF (WIN32 AND HAVE_WINDEBUG)
	SET (WIN32_CXXFLAGS "-mwindows" ) #Prevents process from automatically spawning a console window.
ELSE ()
	# NOTE This might not be the best way, it could overwrite previous WIN32_CXXFLAGS if ever set.
	# LOOK HERE FUTURE YOU IF YOU HAVE PROBLEMS WITH WINDOWS COMPILING!! -- nyanpasu
	SET (WIN32_CXXFLAGS "")
ENDIF ()

# Set compiler flags
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall ${WIN32_CXXFLAGS}")

# Fuck libboost for not providing boost.pc or fucking something
SET (Boost_USE_STATIC_LIBS     ON) 
SET (Boost_USE_MULTITHREADED   ON)  
SET (Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

# Find libraries the proper way
INCLUDE (FindPkgConfig)

PKG_SEARCH_MODULE (LIBTORRENT REQUIRED libtorrent-rasterbar)
PKG_SEARCH_MODULE (LIBGTKMM REQUIRED gtkmm-3.0)
PKG_SEARCH_MODULE (LIBGLIBMM REQUIRED glibmm-2.4)

# Build gt-core
ADD_SUBDIRECTORY (gtorrent-core/src)

# Build gt-gtk
ADD_SUBDIRECTORY (src)

# Uninstall target
CONFIGURE_FILE (
	"${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	"${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	IMMEDIATE @ONLY)

file(COPY assets/gtorrent.png DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets/settings.glade DESTINATION ${CMAKE_BINARY_DIR})

ADD_CUSTOM_TARGET (uninstall
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

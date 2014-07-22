############################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

if (COMMAND cmake_policy)
 cmake_policy (SET CMP0003 NEW)
endif (COMMAND cmake_policy)

PROJECT (gtorrent-gtk)

SET (GTORRENT_VERSION_MAJOR 0)
SET (GTORRENT_VERSION_MINOR 0)
SET (GTORRENT_VERSION_PATCH 2)

############################################################

# Configure version into utils/Version.hpp
SET (VERSION ${GTORRENT_VERSION_MAJOR}.${GTORRENT_VERSION_MINOR}.${GTORRENT_VERSION_PATCH})
configure_file(src/utils/Version.hpp.in Version.hpp @ONLY)

OPTION(WIN_DEBUG "Disable -mwindows for debugging purposes" OFF)
IF (WIN32 AND (NOT WIN_DEBUG))
    SET (WIN32_CXXFLAGS "-mwindows" ) #Prevents process from automatically spawning a console window.
ELSE ()
    SET (WIN32_CXXFLAGS "")
ENDIF ()

# Set compiler flags
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -include all.h -Wall ${WIN32_CXXFLAGS}")

# Fuck libboost for not providing boost.pc or fucking something
SET (Boost_USE_STATIC_LIBS ON) 
SET (Boost_USE_MULTITHREADED ON)  
SET (Boost_USE_STATIC_RUNTIME OFF) 
FIND_PACKAGE(Boost COMPONENTS system REQUIRED)

# Find libraries the proper way
INCLUDE (FindPkgConfig)
PKG_SEARCH_MODULE(LIBTORRENT REQUIRED libtorrent-rasterbar)
PKG_SEARCH_MODULE(LIBGTKMM REQUIRED gtkmm-3.0)
PKG_SEARCH_MODULE(LIBGLIBMM REQUIRED glibmm-2.4)

INCLUDE_DIRECTORIES( ${PROJECT_BINARY_DIR} )

LINK_DIRECTORIES (
    ${Boost_LIBRARY_DIRS}
    ${LIBTORRENT_LIBRARY_DIRS}
    ${LIBGTKMM_LIBRARY_DIRS}
    ${LIBGLIBMM_LIBRARY_DIRS}
	${CMAKE_SOURCE_DIRECTORY}
)

# Create static libbrary

ADD_EXECUTABLE ( ${PROJECT_NAME}
        src/Application.cpp
        src/core/Base.cpp
        src/gtk/Gtk.cpp
        src/gtk/GtkAddMagnetLinkWindow.cpp
        src/gtk/GtkMainWindow.cpp
        src/gtk/GtkTorrentTreeView.cpp
		src/main.cpp
)

ADD_LIBRARY ( gtorrent
		gtorrent-core/src
		gtorrent-core/src/Core.cpp
		gtorrent-core/src/Log.cpp
		gtorrent-core/src/Torrent.cpp
		gtorrent-core/src/utils
)

ADD_DEPENDENCIES(gtorrent-gtk gtorrent)

ADD_DEFINITIONS (
    ${LIBTORRENT_CFLAGS}
    ${LIBGLIBMM_CFLAGS}
    ${LIBGTKMM_CFLAGS}
)

INCLUDE_DIRECTORIES (
    gtorrent-core/include
    gtorrent-core/include/gtorrent
	src
	include
    ${Boost_INCLUDE_DIRS}
    ${LIBTORRENT_INCLUDE_DIRS}
    ${LIBGTKMM_INCLUDE_DIRS}
    ${LIBGLIBMM_INCLUDE_DIRS}
)

TARGET_LINK_LIBRARIES ( gtorrent
    ${CMAKE_SOURCE_DIRECTORY}/libs/libtorrent.a
    ${Boost_LIBRARIES}
    ${LIBTORRENT_LIBRARIES}

)      

TARGET_LINK_LIBRARIES ( gtorrent-gtk
    ${CMAKE_BINARY_DIR}/libgtorrent.a
    ${Boost_LIBRARIES}
    ${LIBTORRENT_LIBRARIES}
    ${LIBGTKMM_LIBRARIES}
    ${LIBGLIBMM_LIBRARIES}
)      


INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${PREFIX}/bin/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

LINK_DIRECTORIES (
	${Boost_LIBRARY_DIRS}
	${LIBTORRENT_LIBRARY_DIRS}
	${LIBGTKMM_LIBRARY_DIRS}
	${LIBGLIBMM_LIBRARY_DIRS}
	${CMAKE_SOURCE_DIRECTORY}
	)

ADD_EXECUTABLE ( ${PROJECT_NAME}
	Application.cpp
	core/Base.cpp
	gtk/Gtk.cpp
	gtk/GtkAddMagnetLinkWindow.cpp
	gtk/GtkMainWindow.cpp
	gtk/GtkTorrentTreeView.cpp
	gtk/GtkTorrentInfoBar.cpp
	gtk/GtkBlockBar.cpp
	gtk/GtkGraph.cpp
	main.cpp
	)

ADD_DEPENDENCIES(gtorrent-gtk gtorrent)

ADD_DEFINITIONS (
	${LIBTORRENT_CFLAGS}
	${LIBGLIBMM_CFLAGS}
	${LIBGTKMM_CFLAGS}
	)

INCLUDE_DIRECTORIES (
	../gtorrent-core/include
	../gtorrent-core/include/gtorrent
	${Boost_INCLUDE_DIRS}
	${LIBTORRENT_INCLUDE_DIRS}
	${LIBGTKMM_INCLUDE_DIRS}
	${LIBGLIBMM_INCLUDE_DIRS}
	)

TARGET_LINK_LIBRARIES ( gtorrent-gtk
	# NOTE Harmful here remember this might lead to trouble in the future
	${CMAKE_BINARY_DIR}/gtorrent-core/src/libgtorrent.a
	${Boost_LIBRARIES}
	${LIBTORRENT_LIBRARIES}
	${LIBGTKMM_LIBRARIES}
	${LIBGLIBMM_LIBRARIES}
	)      

INSTALL(TARGETS ${PROJECT_NAME} EXECUTABLE DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

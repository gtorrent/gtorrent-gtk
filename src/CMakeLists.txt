LINK_DIRECTORIES (
	${Boost_LIBRARY_DIRS}
	${LIBTORRENT_LIBRARY_DIRS}
	${LIBGTKMM_LIBRARY_DIRS}
	${LIBGLIBMM_LIBRARY_DIRS}
	${LIBNOTIFY_LIBRARY_DIRS}
	${CMAKE_SOURCE_DIRECTORY}
	)

INCLUDE_DIRECTORIES (
	../gtorrent-core/include
	../gtorrent-core/include/gtorrent
	${Boost_INCLUDE_DIRS}
	${LIBTORRENT_INCLUDE_DIRS}
	${LIBGTKMM_INCLUDE_DIRS}
	${LIBGLIBMM_INCLUDE_DIRS}
	${LIBNOTIFY_INCLUDE_DIRS}
	)

INCLUDE_DIRECTORIES (${PROJECT_BINARY_DIR})

SET_SOURCE_FILES_PROPERTIES(${PROJECT_BINARY_DIR}/resources.c PROPERTIES GENERATED 1)

ADD_EXECUTABLE ( ${PROJECT_NAME}
	${PROJECT_BINARY_DIR}/resources.c
	Application.cpp
	core/Base.cpp
	gtk/Gtk.cpp
	gtk/GtkAssociationDialog.cpp
	gtk/GtkMainWindow.cpp
	gtk/GtkTorrentTreeView.cpp
	gtk/GtkTorrentInfoBar.cpp
	gtk/GtkBlockBar.cpp
	gtk/GtkGraph.cpp
	gtk/GtkSettingsDialog.cpp
	gtk/GtkPeerTreeView.cpp
	gtk/GtkTrackerTreeView.cpp
	gtk/GtkFileTreeView.cpp
  gtk/utils/FileTree.cpp
  gtk/GtkGeneralBox.cpp
	gtk/GtkPeersBox.cpp
	gtk/GtkPiecesBox.cpp
	gtk/GtkFilesBox.cpp
	gtk/GtkLogBox.cpp
	main.cpp
	)

TARGET_LINK_LIBRARIES ( gtorrent-gtk
	# NOTE Harmful here remember this might lead to trouble in the future
	${CMAKE_BINARY_DIR}/gtorrent-core/src/libgtorrent.a
	${Boost_LIBRARIES}
	${LIBTORRENT_LIBRARIES}
	${LIBGTKMM_LIBRARIES}
	${LIBGLIBMM_LIBRARIES}
	${LIBNOTIFY_LIBRARIES}
	)

ADD_DEPENDENCIES(gtorrent-gtk gtorrent assets)

ADD_DEFINITIONS (
	${LIBTORRENT_CFLAGS}
	${LIBGLIBMM_CFLAGS}
	${LIBGTKMM_CFLAGS}
	${LIBNOTIFY_CFLAGS}
	)

INSTALL(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/ PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

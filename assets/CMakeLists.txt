# Generate resource header and source
ADD_CUSTOM_COMMAND (
	OUTPUT ${PROJECT_BINARY_DIR}/resources.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Generating resources.c..."
	COMMAND glib-compile-resources ARGS --target=${PROJECT_BINARY_DIR}/resources.c --generate-source resources.xml
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/window_main.ui
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/widget_panel_torrent.ui
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dialog_association.ui
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dialog_settings.ui
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dialog_rss.ui
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dialog_generic.ui
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dialog_function.ui
	)

ADD_CUSTOM_COMMAND (
	OUTPUT ${PROJECT_BINARY_DIR}/style.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Generating style.c..."
	COMMAND xxd -i style.css > ${PROJECT_BINARY_DIR}/style.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/style.css
	)

ADD_CUSTOM_TARGET (assets ALL
	DEPENDS ${PROJECT_BINARY_DIR}/resources.c
	DEPENDS ${PROJECT_BINARY_DIR}/style.c
	)

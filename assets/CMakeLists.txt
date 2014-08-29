# Generate resource header and source
ADD_CUSTOM_COMMAND (
	OUTPUT ${PROJECT_BINARY_DIR}/resources.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND glib-compile-resources ARGS --target=${PROJECT_BINARY_DIR}/resources.c --generate-source resources.xml
	COMMENT "Generating resources.c..."
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources.xml
	)

ADD_CUSTOM_TARGET (assets ALL
	DEPENDS ${PROJECT_BINARY_DIR}/resources.c
	)

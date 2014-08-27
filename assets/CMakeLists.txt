#CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

#PROJECT (assets)

add_custom_command (
	OUTPUT resources.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND glib-compile-resources --generate-source --target=resources.c resources.xml
)

add_custom_command (
	OUTPUT resources.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND glib-compile-resources --generate-header --target=resources.h resources.xml
)

add_custom_command (
	OUTPUT ${PROJECT_SOURCE_DIR}/src/gtk/resources.c
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources.c ${PROJECT_SOURCE_DIR}/src/gtk/resources.c
	DEPENDS resources.c
)

add_custom_command (
	OUTPUT ${PROJECT_SOURCE_DIR}/src/gtk/resources.h
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/resources.h ${PROJECT_SOURCE_DIR}/src/gtk/resources.h
	DEPENDS resources.h
)

add_custom_target(assets ALL
    DEPENDS ${PROJECT_SOURCE_DIR}/src/gtk/resources.c
    DEPENDS ${PROJECT_SOURCE_DIR}/src/gtk/resources.h)

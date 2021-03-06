PROJECT(SPACENAVIGATOR)

IF(NOT BALL_HAS_VIEW)
	MESSAGE(STATUS "BALL is configured to not build the VIEW components. Disabling the SpaceNavigator plugin.")
	RETURN()
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

FIND_PACKAGE(SpaceNavigator)

IF(NOT SPNAV_FOUND)
	MESSAGE(STATUS "Could not find SpaceNavigator libraries. Disabling the SpaceNavigator plugin")
	RETURN()
ENDIF()

SET(DIRECTORY source)
### list all filenames of the directory here ###
SET(SOURCES_LIST
	${DIRECTORY}/spaceNavigatorDriver.C
	${DIRECTORY}/spaceNavigatorPlugin.C
)

SET(RCC_SOURCES_LIST
	${DIRECTORY}/logo.qrc
)

SET(DIRECTORY include)
### the list of all files requiring a moc run ###
SET(MOC_SOURCES_LIST
	${DIRECTORY}/spaceNavigatorPlugin.h
)
#IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#	LIST(APPEND RESOURCES_LIST ${CMAKE_SOURCE_DIR}/cmake/installer/BALLView.icns)
#	SET(MACOSX_BUNDLE_ICON_FILE BALLView.icns)
#	SET_SOURCE_FILES_PROPERTIES(${CMAKE_SOURCE_DIR}/cmake/installer/BALLView.icns
#			PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
#ENDIF()


QT4_WRAP_CPP(MOC_OUTFILES ${MOC_SOURCES_LIST})
QT4_ADD_RESOURCES(RCC_OUTFILES ${RCC_SOURCES_LIST})
QT4_WRAP_UI(UIC_OUTFILES ${UI_SOURCES_LIST})

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR})
INCLUDE_DIRECTORIES(${SPNAV_INCLUDE_DIRS})

ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DQT_PLUGIN)
ADD_DEFINITIONS(-DQT_NO_DEBUG)
ADD_DEFINITIONS(-DQT_SHARED)

ADD_LIBRARY(pluginSpaceNavigator SHARED
	${SOURCES_LIST}
	${MOC_OUTFILES}
	${RCC_OUTFILES}
	${UIC_OUTFILES}
)

SET_TARGET_PROPERTIES(pluginSpaceNavigator PROPERTIES PREFIX "")

INSTALL(TARGETS pluginSpaceNavigator
	COMPONENT "${COMPONENT_PLUGINS}"
	RUNTIME DESTINATION "${BALL_PLUGIN_INSTALL_DIRECTORY}"
	LIBRARY DESTINATION "${BALL_PLUGIN_INSTALL_DIRECTORY}"
)

TARGET_LINK_LIBRARIES(pluginSpaceNavigator BALL VIEW ${SPNAV_LIBRARIES})

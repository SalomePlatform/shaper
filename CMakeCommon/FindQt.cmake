CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

IF(WIN32)
    SET(Q_WS_WIN ON)
    SET(QT_USE_QTMAIN ON)
ELSE()
    SET(Q_WS_X11 ON)
ENDIF()

# Widgets finds its own dependencies (QtGui and QtCore).
FIND_PACKAGE(Qt4 REQUIRED)

IF(Qt4_FOUND)
    SET(QT_VERSION_STRING "${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")
	MESSAGE(STATUS "Found Qt version: "  ${QT_VERSION_STRING})
ENDIF()

INCLUDE(${QT_USE_FILE})

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
INCLUDE_DIRECTORIES(${QT_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
#SET(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")
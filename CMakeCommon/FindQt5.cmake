CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)
#Append Qt5's install prefix into CMAKE_MODULE_PATH
SET(CMAKE_MODULE_PATH "$ENV{QTDIR}" ${CMAKE_MODULE_PATH})

SET(CMAKE_AUTOMOC ON)

# Widgets finds its own dependencies (QtGui and QtCore).
FIND_PACKAGE(Qt5Widgets REQUIRED)
FIND_PACKAGE(Qt5LinguistTools REQUIRED)

IF(Qt5Widgets_FOUND)
	MESSAGE(STATUS "Found Qt version:" ${Qt5Widgets_VERSION_STRING})
ENDIF()

# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
INCLUDE_DIRECTORIES(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
ADD_DEFINITIONS(${Qt5Widgets_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
SET(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

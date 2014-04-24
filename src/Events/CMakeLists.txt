INCLUDE(Common)

SET(PROJECT_HEADERS
    Events.h
    Events_Message.h
    Events_Listener.h
    Events_Loop.h
)

SET(PROJECT_SOURCES
    Events_Message.cpp
    Events_Listener.cpp
    Events_Loop.cpp
)

ADD_DEFINITIONS(-DEVENTS_EXPORTS)
ADD_LIBRARY(Events SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

TARGET_LINK_LIBRARIES(Events ${PROJECT_LIBRARIES})

INSTALL(TARGETS Events DESTINATION bin)

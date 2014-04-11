INCLUDE(Common)

SET(PROJECT_HEADERS
    Event.h
    Event_Message.h
    Event_Listener.h
    Event_Loop.h
)

SET(PROJECT_SOURCES
    Event_Message.cxx
    Event_Listener.cxx
    Event_Loop.cxx
)

ADD_DEFINITIONS(-DEVENT_EXPORTS)
ADD_LIBRARY(Event SHARED ${PROJECT_SOURCES} ${PROJECT_HEADERS})

TARGET_LINK_LIBRARIES(Event ${PROJECT_LIBRARIES})

INSTALL(TARGETS Event DESTINATION bin)

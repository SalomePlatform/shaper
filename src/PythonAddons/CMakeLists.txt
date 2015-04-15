## Copyright (C) 2014-20xx  OPEN CASCADE


SET(CMAKE_AUTOMOC ON)

INSTALL(FILES addons_Features.py addons_Features.xml DESTINATION plugins)


INSTALL(FILES __init__.py DESTINATION addons)
INSTALL(DIRECTORY macros DESTINATION addons)

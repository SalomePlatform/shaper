## Copyright (C) 2014-20xx  OPEN CASCADE


SET(CMAKE_AUTOMOC ON)

INSTALL(DIRECTORY extension geom modeler DESTINATION PythonAPI)

# --------- Unit tests -----------
INCLUDE(UnitTest)

ADD_UNIT_TESTS(
  TestModeler.py
  TestSketcherAddPoint.py
  TestSketcherAddLine.py
  TestSketcherAddArc.py
  TestSketcherAddCircle.py
  TestSketcherSetCoincident.py
  TestSketcherSetParallel.py
  ) 

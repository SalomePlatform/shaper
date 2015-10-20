## Copyright (C) 2014-20xx  OPEN CASCADE


SET(CMAKE_AUTOMOC ON)

INSTALL(DIRECTORY extension geom model DESTINATION PythonAPI)
INSTALL(FILES shaper.py DESTINATION PythonAPI)

# --------- Unit tests -----------
INCLUDE(UnitTest)

ADD_UNIT_TESTS(
  TestShaper.py
  TestModel.py
  TestSketcherAddPoint.py
  TestSketcherAddLine.py
  TestSketcherAddArc.py
  TestSketcherAddCircle.py
  TestSketcherSetCoincident.py
  TestSketcherSetParallel.py
  TestSketcherSetPerpendicular.py
  TestSketcherSetHorizontal.py
  TestSketcherSetVertical.py
  TestSketcherSetLength.py
  TestSketcherSetRadius.py
  TestSketcherSetAngle.py
  TestSketcherSetEqual.py
  TestSketcherSetFillet.py
  ) 

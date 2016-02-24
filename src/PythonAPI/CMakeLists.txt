## Copyright (C) 2014-20xx  OPEN CASCADE


SET(CMAKE_AUTOMOC ON)

INSTALL(DIRECTORY extension geom model examples DESTINATION ${SHAPER_INSTALL_PYTHON_API})
INSTALL(FILES shaper.py DESTINATION ${SHAPER_INSTALL_PYTHON_API})

# --------- Unit tests -----------
INCLUDE(UnitTest)

ADD_UNIT_TESTS(
  TestPythonAPI.py
  TestModel.py

  TestSketcherAddPoint.py
  TestSketcherAddLine.py
  TestSketcherAddArc.py
  TestSketcherAddCircle.py
  TestSketcherAddMirror.py
  TestSketcherSetCoincident.py
  TestSketcherSetTangent.py
  TestSketcherSetParallel.py
  TestSketcherSetPerpendicular.py
  TestSketcherSetHorizontal.py
  TestSketcherSetVertical.py
  TestSketcherSetRigid.py
  TestSketcherSetLength.py
  TestSketcherSetRadius.py
  TestSketcherSetAngle.py
  TestSketcherSetEqual.py
  TestSketcherSetFillet.py

  TestFeatures.py
  TestFeaturesExtrusion.py
  TestFeaturesRevolution.py

  TestMakeBrick1.py
  TestMakeBrick2.py
  TestMakeBrick3.py
  TestPlatine.py
  )

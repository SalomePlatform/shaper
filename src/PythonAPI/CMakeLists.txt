## Copyright (C) 2014-20xx  OPEN CASCADE

INSTALL(DIRECTORY geom model examples DESTINATION ${SHAPER_INSTALL_PYTHON_API})
INSTALL(FILES __init__.py DESTINATION ${SHAPER_INSTALL_PYTHON_API})

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
  TestSketcherSetFixed.py
  TestSketcherSetLength.py
  TestSketcherSetRadius.py
  #TestSketcherSetAngle.py
  TestSketcherSetEqual.py
  TestSketcherSetFillet.py

  TestFeatures.py
  TestFeaturesExtrusion.py
  #TestFeaturesRevolution.py
  
  TestPrimitivesBox.py

  TestMakeBrick1.py
  TestMakeBrick2.py
  TestPlatine.py
  )

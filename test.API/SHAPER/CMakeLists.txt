## Copyright (C) 2014-20xx CEA/DEN, EDF R&D
INCLUDE(UnitTest)

ADD_UNIT_TESTS_API(
  Primitives/TestBox.py
  Primitives/TestCylinder.py
  Primitives/TestAPI_Box.py
  Primitives/TestAPI_Cylinder.py
  GDML/TestConeSegment.py
  GDML/TestAPI_ConeSegment.py
  Transformations/TestScale.py
  Transformations/TestSymmetry.py
  Transformations/TestTranslation.py
  Transformations/TestAPI_Scale.py
  Transformations/TestAPI_Symmetry.py
  Transformations/TestAPI_Translation.py
)

# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

INSTALL(DIRECTORY geom model examples DESTINATION ${SHAPER_INSTALL_PYTHON_API})
INSTALL(FILES __init__.py DESTINATION ${SHAPER_INSTALL_PYTHON_API})
#INSTALL(FILES __init__.py DESTINATION ${_pydir}/salome)
INSTALL(FILES initConfig.py DESTINATION ${SHAPER_INSTALL_PYTHON_API})

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
  TestSketcherSetAngle.py
  TestSketcherSetEqual.py
  TestSketcherSetFillet.py

  TestFeatures.py
  TestFeaturesExtrusion.py
  TestFeaturesRevolution.py

  TestPrimitivesBox.py

  TestMakeBrick1.py
  TestMakeBrick2.py
  TestPlatine.py

  Test2044.py
  )

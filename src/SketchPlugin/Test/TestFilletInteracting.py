# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

"""
    TestFilletInteracting.py
    Unit test of SketchPlugin_Fillet feature.
    In scope of interaction with another constraints applied to filleted features.
"""

from GeomAPI import *
from GeomDataAPI import *
from ModelAPI import *
from SketchAPI import *
import math
import unittest
from salome.shaper import model

__updated__ = "2017-03-06"

def isArcLineSmooth(theArc, theLine, theTolerance):
  aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
  aDistance = model.distancePointLine(aCenter, theLine)
  aRadius = arcRadius(theArc)
  return math.fabs(aRadius - aDistance) < theTolerance

def isArcArcSmooth(theArc1, theArc2, theTolerance):
  aCenter1 = geomDataAPI_Point2D(theArc1.attribute("center_point"))
  aCenter2 = geomDataAPI_Point2D(theArc2.attribute("center_point"))
  aDistance = model.distancePointPoint(aCenter1, aCenter2)
  aRadius1 = arcRadius(theArc1)
  aRadius2 = arcRadius(theArc2)
  aRadSum = aRadius1 + aRadius2
  aRadDiff = math.fabs(aRadius1 - aRadius2)
  return math.fabs(aDistance - aRadSum) < theTolerance or math.fabs(aDistance - aRadDiff) < theTolerance

def arcRadius(theArc):
  aCenter = geomDataAPI_Point2D(theArc.attribute("center_point"))
  aStart = geomDataAPI_Point2D(theArc.attribute("start_point"))
  return model.distancePointPoint(aCenter, aStart)



class TestFilletInteracting(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myTolerance = 1.e-6
    self.myDOF = 0

  def tearDown(self):
    model.end()
    self.assertTrue(model.checkPythonDump())
    model.reset()


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def collectFeatures(self):
    self.myFeatures = {}
    for aSubObj in self.mySketch.features().list():
      aFeature = ModelAPI.ModelAPI_Feature.feature(aSubObj)
      if aFeature is not None:
        if self.myFeatures.get(aFeature.getKind()) == None:
          self.myFeatures[aFeature.getKind()] = 1
        else:
          self.myFeatures[aFeature.getKind()] += 1

  def checkNbFeatures(self, theFeatureKind, theFeatureCount):
    if theFeatureCount == 0:
      self.assertIsNone(self.myFeatures.get(theFeatureKind))
    else:
      self.assertIsNotNone(self.myFeatures.get(theFeatureKind), "No features of kind {0} but expected {1}".format(theFeatureKind, theFeatureCount))
      self.assertEqual(self.myFeatures[theFeatureKind], theFeatureCount, "Observed number of {0} is {1} but expected {2}".format(theFeatureKind, self.myFeatures[theFeatureKind], theFeatureCount))

  def checkFillet(self):
    aPtPtCoincidences = self.getCoincidences()
    for coinc in aPtPtCoincidences:
      aConnectedFeatures = self.connectedFeatures(coinc)
      self.assertEqual(len(aConnectedFeatures), 2)
      if aConnectedFeatures[0].getKind() == "SketchArc":
        if aConnectedFeatures[1].getKind() == "SketchArc":
          self.assertTrue(isArcArcSmooth(aConnectedFeatures[0], aConnectedFeatures[1], self.myTolerance))
        elif aConnectedFeatures[1].getKind() == "SketchLine":
          self.assertTrue(isArcLineSmooth(aConnectedFeatures[0], aConnectedFeatures[1], self.myTolerance))
      elif aConnectedFeatures[0].getKind() == "SketchLine" and aConnectedFeatures[1].getKind() == "SketchArc":
        self.assertTrue(isArcLineSmooth(aConnectedFeatures[1], aConnectedFeatures[0], self.myTolerance))

  def getCoincidences(self):
    aCoincidences = []
    for aSubObj in self.mySketch.features().list():
      aSubFeature = ModelAPI.ModelAPI_Feature.feature(aSubObj)
      if aSubFeature is not None and aSubFeature.getKind() == "SketchConstraintCoincidence":
        anEntityA = aSubFeature.refattr("ConstraintEntityA")
        anEntityB = aSubFeature.refattr("ConstraintEntityB")
        if not anEntityA.isObject() and not anEntityB.isObject():
          aCoincidences.append(aSubFeature)
    return aCoincidences

  def connectedFeatures(self, theCoincidence):
    anEntityA = theCoincidence.refattr("ConstraintEntityA")
    anEntityB = theCoincidence.refattr("ConstraintEntityB")
    aFeatureA = ModelAPI.ModelAPI_Feature.feature(anEntityA.attr().owner())
    aFeatureB = ModelAPI.ModelAPI_Feature.feature(anEntityB.attr().owner())
    return [aFeatureA, aFeatureB]


  def test_fillet_two_lines(self):
    """ Test 1. Fillet on two connected lines
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_wrong_fillet_two_lines(self):
    """ Test 2. Check the fillet is wrong on two connected lines when selecting incorrect point
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    aFillet = self.mySketch.setFillet(aSketchLineA.endPoint())
    model.do()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 0) # no arcs should be created
    self.checkNbFeatures("SketchConstraintCoincidence", 1) # number of coincidences should not change
    self.checkNbFeatures("SketchConstraintTangent", 0) # no tangencies should not be created
    self.checkNbFeatures("SketchFillet", 1) # fillet feature should still exist. it should be wrong

    # sketch is invalid, so, no results at all
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [])

    # remove fillet for correct python dump
    self.myDocument.removeFeature(aFillet.feature())

  def test_fillet_arc_line(self):
    """ Test 3. Fillet on connected arc and line
    """
    aSketchLine = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchArc = self.mySketch.addArc(20., 10., 20., 20., 10., 10., False)
    self.myDOF += 9
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLine.startPoint(), aSketchArc.endPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLine.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 1)
    self.checkNbFeatures("SketchArc", 2)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_two_arcs(self):
    """ Test 4. Fillet on two connected arcs
    """
    aSketchArc1 = self.mySketch.addArc(20., 0., 20., 20., 10., 17.32050807568877293, False)
    aSketchArc2 = self.mySketch.addArc(20., 34.64101615137754586, 20., 14.64101615137754586, 10., 17.32050807568877293, True)
    self.myDOF += 10
    self.checkDOF()
    self.mySketch.setCoincident(aSketchArc1.endPoint(), aSketchArc2.endPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchArc1.endPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 0)
    self.checkNbFeatures("SketchArc", 3)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_horizontal_vertical(self):
    """ Test 5. Fillet on two connected lines in case of Horizontal or Vertical constraints applied
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setHorizontal(aSketchLineA.result())
    self.mySketch.setVertical(aSketchLineB.result())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintHorizontal", 1)
    self.checkNbFeatures("SketchConstraintVertical", 1)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_orthogonal(self):
    """ Test 6. Fillet on two connected lines in case of Perpendicular constraint applied
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setPerpendicular(aSketchLineA.result(), aSketchLineB.result())
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintPerpendicular", 1)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_parallel(self):
    """ Test 7. Fillet on two connected lines in case of Parallel constraint applied
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    # third line to apply parallel constraint
    aSketchLineC = self.mySketch.addLine(10., 0., 20., 5.)
    self.myDOF += 4
    self.mySketch.setParallel(aSketchLineB.result(), aSketchLineC.result())
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 3)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintParallel", 1)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [4])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [8])

  def test_fillet_with_equal_lines(self):
    """ Test 8. Fillet on two connected lines in case of Equal constraint applied
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setEqual(aSketchLineA.result(), aSketchLineB.result())
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 2 # Equal has been removed
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintEqual", 0) # Equal constraint expected to be removed
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_equal_arcs(self):
    """ Test 9. Fillet on two connected arcs in case of Equal constraint applied
    """
    aSketchArc1 = self.mySketch.addArc(20., 0., 20., 20., 10., 17.32050807568877293, False)
    aSketchArc2 = self.mySketch.addArc(20., 34.64101615137754586, 20., 14.64101615137754586, 10., 17.32050807568877293, True)
    self.myDOF += 10
    self.checkDOF()
    self.mySketch.setCoincident(aSketchArc1.endPoint(), aSketchArc2.endPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setEqual(aSketchArc1.results()[1], aSketchArc2.results()[1])
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchArc1.endPoint())
    self.myDOF += 2 # Equal has been removed
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 0)
    self.checkNbFeatures("SketchArc", 3)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintEqual", 0) # Equal constraint expected to be removed
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_length(self):
    """ Test 10. Fillet on two connected lines in case of Length constraint applied
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setLength(aSketchLineA.result(), 15.)
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 4) # Additionally 2 coincidences for apex with fillet objects
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintLength", 0) # Length constraint expected to be removed
    self.checkNbFeatures("SketchConstraintDistance", 1) # Distance constraint should appear instead of Length
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_radius(self):
    """ Test 11. Fillet on connected arc and line in case of Radius constraint is applied to arc
    """
    aSketchLine = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchArc = self.mySketch.addArc(0., 10., 0., 20., 10., 10., True)
    self.myDOF += 9
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLine.startPoint(), aSketchArc.endPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setRadius(aSketchArc.results()[1], 12.)
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLine.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 1)
    self.checkNbFeatures("SketchArc", 2)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintRadius", 1)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_distance(self):
    """ Test 12. Fillet on two connected lines in case of Distance constraint applied
    """
    aSketchLineA = self.mySketch.addLine(20, 20, 70, 20)
    aSketchLineB = self.mySketch.addLine(70, 20, 70, 53.16624790355412)
    aSketchLineC = self.mySketch.addLine(70, 53.16624790355412, 20, 20)
    self.myDOF += 12
    model.do()
    self.checkDOF()
    # coincidences
    self.mySketch.setCoincident(aSketchLineA.endPoint(), aSketchLineB.startPoint())
    self.mySketch.setCoincident(aSketchLineB.endPoint(), aSketchLineC.startPoint())
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineC.endPoint())
    self.myDOF -= 6
    model.do()
    self.checkDOF()
    # other constraints
    self.mySketch.setHorizontal(aSketchLineA.result())
    self.mySketch.setVertical(aSketchLineB.result())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    # construction point
    aProjection = self.mySketch.addProjection(model.selection("VERTEX", "Origin"), False)
    aSketchPoint = SketchAPI_Point(aProjection.createdFeature())
    model.do()
    # distances
    self.mySketch.setLength(aSketchLineA.result(), 50)
    self.mySketch.setDistance(aSketchLineA.startPoint(), aSketchLineC.startPoint(), 60, True)
    self.mySketch.setHorizontalDistance(aSketchPoint.coordinates(), aSketchLineA.startPoint(), 20)
    self.mySketch.setVerticalDistance(aSketchPoint.coordinates(), aSketchLineC.endPoint(), 20)
    self.myDOF -= 4
    model.do()
    self.checkDOF()

    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 3)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 6) # Additionally 2 coincidences for apex with fillet objects
    self.checkNbFeatures("SketchConstraintHorizontal", 1)
    self.checkNbFeatures("SketchConstraintVertical", 1)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintLength", 0) # Length translated to Distance
    self.checkNbFeatures("SketchConstraintDistance", 2) # Length translated to Distance
    self.checkNbFeatures("SketchConstraintDistanceHorizontal", 1)
    self.checkNbFeatures("SketchConstraintDistanceVertical", 1)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [4])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [8])

  def test_fillet_with_fixed_point(self):
    """ Test 13. Fillet on two connected lines in case of Fixed constraint applied to the fillet point
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFixed(aSketchLineA.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 3 # Fixed constraint has been removed
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintFixed", 0) # Fixed constraint expected to be removed
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_fixed_line(self):
    """ Test 14. Fillet on two connected lines in case of Fixed constraint applied to one of lines
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setFixed(aSketchLineA.result())
    self.myDOF -= 4
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintLength", 0) # Fixed constraint expected to be kept
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])

  def test_fillet_with_angle(self):
    """ Test 15. Fillet on two connected lines in case of Perpendicular constraint applied
    """
    aSketchLineA = self.mySketch.addLine(10., 10., 20., 10.)
    aSketchLineB = self.mySketch.addLine(10., 10., 10., 20.)
    self.myDOF += 8
    self.checkDOF()
    self.mySketch.setCoincident(aSketchLineA.startPoint(), aSketchLineB.startPoint())
    self.myDOF -= 2
    model.do()
    self.checkDOF()
    self.mySketch.setAngle(aSketchLineA.result(), aSketchLineB.result(), 60.)
    self.myDOF -= 1
    model.do()
    self.checkDOF()
    self.mySketch.setFillet(aSketchLineA.startPoint())
    self.myDOF += 1
    model.do()
    self.checkFillet()
    self.checkDOF()

    self.collectFeatures()
    self.checkNbFeatures("SketchLine", 2)
    self.checkNbFeatures("SketchArc", 1)
    self.checkNbFeatures("SketchConstraintCoincidence", 2)
    self.checkNbFeatures("SketchConstraintTangent", 2)
    self.checkNbFeatures("SketchConstraintAngle", 1)
    self.checkNbFeatures("SketchFillet", 0)

    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.FACE, [0])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.EDGE, [3])
    model.testNbSubShapes(self.mySketch, GeomAPI_Shape.VERTEX, [6])


if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"

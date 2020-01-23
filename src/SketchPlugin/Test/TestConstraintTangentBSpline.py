# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
    Test constraint "Tangent" applied to B-spline and another entity
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from GeomAlgoAPI import *
from SketchAPI import *

__updated__ = "2020-01-22"

class TestTangentBSpline(unittest.TestCase):
  def setUp(self):
    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    self.myPoles = [GeomAPI_Pnt2d(-10, -30), GeomAPI_Pnt2d(20, -15), GeomAPI_Pnt2d(-10, 0), GeomAPI_Pnt2d(20, 15), GeomAPI_Pnt2d(-10, 30)]
    self.myWeights = [1, 3, 5, 3, 1]
    self.mySpline = self.mySketch.addSpline(self.myPoles, self.myWeights)
    self.myControlPoles = self.mySpline.controlPoles(auxiliary = [0, 1, 2, 3, 4])
    self.myControlLines = self.mySpline.controlPolygon(auxiliary = [0, 1, 2, 3])
    model.do()

    self.myExpectedFailure = False
    self.myDOF = len(self.myPoles) * 2
    self.myNbPoints = len(self.myPoles)
    self.myNbLines = len(self.myPoles) - 1
    self.myNbArcs = 0
    self.myNbCircles = 0
    self.myNbEllipses = 0
    self.myNbEllipticArcs = 0
    self.myNbBSplines = 1
    self.myNbInternals = len(self.myPoles) * 3 - 2
    self.myNbCoincidence = 0
    self.myNbTangency = 0

  def tearDown(self):
    model.end()
    if self.myExpectedFailure:
      assert(self.mySketch.solverError() != ""), "PlaneGCS limitation: if you see this message, then PlaneGCS has solved the set of constraints correctly"
      model.undo()
    else:
      self.checkDOF()
      model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
      model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
      model.testNbSubFeatures(self.mySketch, "SketchArc", self.myNbArcs)
      model.testNbSubFeatures(self.mySketch, "SketchCircle", self.myNbCircles)
      model.testNbSubFeatures(self.mySketch, "SketchEllipse", self.myNbEllipses)
      model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", self.myNbEllipticArcs)
      model.testNbSubFeatures(self.mySketch, "SketchBSpline", self.myNbBSplines)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", self.myNbInternals)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", self.myNbCoincidence)
      model.testNbSubFeatures(self.mySketch, "SketchConstraintTangent", self.myNbTangency)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def assertTangentFeatures(self, theFeature1, theFeature2):
    shapes = [theFeature1.results()[-1].resultSubShapePair()[0].shape(),
              theFeature2.results()[-1].resultSubShapePair()[0].shape()]
    for s in shapes:
      e = shapeToEdge(s)
      if e.isLine() or e.isArc() or e.isEllipse():
        params = e.getRange()
        e.setRange(params[0] - 100, params[1] + 100)
    # TODO (azv): complete checking the tangent curves

  def assertPointLineDistance(self, thePoint, theLine, theExpectedDistance = 0):
    dist = model.distancePointLine(thePoint, theLine)
    self.assertAlmostEqual(dist, theExpectedDistance)

  def assertTangentLineCircle(self, theLine, theCircle):
    self.assertPointLineDistance(theCircle.center(), theLine, theCircle.radius().value())

  def assertTangentLineEllipse(self, theLine, theEllipse):
    aLine = GeomAPI_Lin2d(theLine.startPoint().pnt(), theLine.endPoint().pnt())
    projF1 = aLine.project(theEllipse.firstFocus().pnt())
    projF2 = aLine.project(theEllipse.secondFocus().pnt())

    distF1P1 = model.distancePointPoint(theEllipse.firstFocus(), projF1)
    distF2P2 = model.distancePointPoint(theEllipse.secondFocus(), projF2)

    tgPoint = GeomAPI_Pnt2d((projF1.x() * distF2P2 + projF2.x() * distF1P1) / (distF1P1 + distF2P2), (projF1.y() * distF2P2 + projF2.y() * distF1P1) / (distF1P1 + distF2P2))
    distF1T = model.distancePointPoint(theEllipse.firstFocus(), tgPoint)
    distF2T = model.distancePointPoint(theEllipse.secondFocus(), tgPoint)
    self.assertAlmostEqual(distF1T + distF2T, 2 * theEllipse.majorRadius().value())


  def test_line_tangent(self):
    """ Test 1. Set tangency between B-spline and a line
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentFeatures(aLine, self.mySpline)

  def test_circle_tangent(self):
    """ Test 2. Set tangency between B-spline and a circle
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myNbCircles += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aCircle.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentFeatures(aCircle, self.mySpline)

  def test_arc_tangent(self):
    """ Test 3. Set tangency between B-spline and an arc
    """
    anArc = self.mySketch.addArc(10, 10, 20, 10, 10, 20, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentFeatures(anArc, self.mySpline)

  def test_ellipse_tangent(self):
    """ Test 4. Set tangency between B-spline and an ellipse
    """
    anEllipse = self.mySketch.addEllipse(10, 10, 20, 10, 7)
    self.myNbEllipses += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anEllipse.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentFeatures(anEllipse, self.mySpline)

  def test_elliptic_arc_tangent(self):
    """ Test 5. Set tangency between B-spline and an elliptic arc
    """
    anEllipticArc = self.mySketch.addEllipticArc(10, 10, 20, 10, 22.2065556157337, 10, 10, 17, True)
    self.myNbEllipticArcs += 1
    self.myDOF += 7
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anEllipticArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentFeatures(anEllipticArc, self.mySpline)

  def test_spline_tangent(self):
    """ Test 6. Set tangency between two B-spline curves
    """
    aSpline = self.mySketch.addSpline([(50, -20), (40, 0), (50, 20)])
    self.myNbBSplines += 1
    self.myDOF += aSpline.poles().size() * 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aSpline.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    #self.assertTangentFeatures(aSpline, self.mySpline)
    self.myExpectedFailure = True


  def test_line_tangent_coincident_by_pole(self):
    """ Test 7. Set tangency between B-spline and a line coincident with B-spline start point
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), aLine.result())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertPointLineDistance(aLine.startPoint(), self.myControlLines[0])
    self.assertPointLineDistance(aLine.endPoint(), self.myControlLines[0])

  def test_circle_tangent_coincident_by_pole(self):
    """ Test 8. Set tangency between B-spline and a circle coincident with B-spline end point
    """
    aCircle = self.mySketch.addCircle(10, 10, 20)
    self.myNbCircles += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), aCircle.defaultResult())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aCircle.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineCircle(SketchAPI_Line(self.myControlLines[0]), aCircle)

  def test_arc_tangent_coincident_by_pole(self):
    """ Test 9. Set tangency between B-spline and an arc coincident with B-spline end point
    """
    anArc = self.mySketch.addArc(10, 10, 20, 10, 10, 20, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setCoincident(self.mySpline.endPoint(), anArc.defaultResult())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineCircle(SketchAPI_Line(self.myControlLines[-1]), anArc)

  def test_ellipse_tangent_coincident_by_pole(self):
    """ Test 10. Set tangency between B-spline and an ellipse coincident with B-spline start point
    """
    anEllipse = self.mySketch.addEllipse(10, 10, 20, 10, 7)
    self.myNbEllipses += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), anEllipse.defaultResult())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anEllipse.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(SketchAPI_Line(self.myControlLines[0]), anEllipse)

  def test_elliptic_arc_tangent_coincident_by_pole(self):
    """ Test 11. Set tangency between B-spline and an elliptic arc coincident with B-spline start point
    """
    anEllipticArc = self.mySketch.addEllipticArc(10, 10, 20, 10, 22.2065556157337, 10, 10, 17, True)
    self.myNbEllipticArcs += 1
    self.myDOF += 7
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), anEllipticArc.defaultResult())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anEllipticArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(SketchAPI_Line(self.myControlLines[0]), anEllipticArc)


  def test_line_tangent_coincident_by_boundaries(self):
    """ Test 12. Set tangency between B-spline and a line, coincident by their start points
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), aLine.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertPointLineDistance(aLine.endPoint(), self.myControlLines[0])

  def test_arc_tangent_coincident_by_boundaries(self):
    """ Test 13. Set tangency between B-spline and an arc, coincident by their start points
    """
    anArc = self.mySketch.addArc(10, 10, 20, 10, 10, 20, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), anArc.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineCircle(SketchAPI_Line(self.myControlLines[0]), anArc)

  def test_elliptic_arc_tangent_coincident_by_boundaries(self):
    """ Test 14. Set tangency between B-spline and an elliptic arc, coincident by their start points
    """
    anEllipticArc = self.mySketch.addEllipticArc(10, -10, 20, -10, 22.2065556157337, -10, 10, 3, True)
    self.myNbEllipticArcs += 1
    self.myDOF += 7
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), anEllipticArc.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anEllipticArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(SketchAPI_Line(self.myControlLines[0]), anEllipticArc)

  def test_spline_tangent_coincident_by_boundaries(self):
    """ Test 15. Set tangency between two B-spline curves coincident with B-spline start point
    """
    aSpline = self.mySketch.addSpline([(50, -20), (40, 0), (50, 20)])
    self.myNbBSplines += 1
    self.myDOF += aSpline.poles().size() * 2
    model.do()

    self.mySketch.setCoincident(self.mySpline.startPoint(), aSpline.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aSpline.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    #self.assertPointLineDistance(aSpline.poles()[1], self.myControlLines[0])
    self.myExpectedFailure = True


  def test_line_tangent_coincident_by_aux(self):
    """ Test 16. Set tangency between B-spline and a line, coincident by their start points
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setCoincident(SketchAPI_Point(self.myControlPoles[0]).coordinates(), aLine.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertPointLineDistance(aLine.endPoint(), self.myControlLines[0])

  def test_arc_tangent_coincident_by_aux(self):
    """ Test 17. Set tangency between B-spline and an arc, coincident by their start points
    """
    anArc = self.mySketch.addArc(10, 10, 20, 10, 10, 20, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setCoincident(SketchAPI_Point(self.myControlPoles[0]).coordinates(), anArc.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineCircle(SketchAPI_Line(self.myControlLines[0]), anArc)

  def test_elliptic_arc_tangent_coincident_by_aux(self):
    """ Test 18. Set tangency between B-spline and an elliptic arc, coincident by their start points
    """
    anEllipticArc = self.mySketch.addEllipticArc(10, 10, 20, 10, 22.2065556157337, 10, 10, 17, True)
    self.myNbEllipticArcs += 1
    self.myDOF += 7
    model.do()

    self.mySketch.setCoincident(SketchAPI_Point(self.myControlPoles[0]).coordinates(), anEllipticArc.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), anEllipticArc.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(SketchAPI_Line(self.myControlLines[0]), anEllipticArc)

  def test_spline_tangent_coincident_by_aux(self):
    """ Test 19. Set tangency between two B-spline curves coincident with B-spline start point
    """
    aSpline = self.mySketch.addSpline([(50, -20), (40, 0), (50, 20)])
    self.myNbBSplines += 1
    self.myDOF += aSpline.poles().size() * 2
    model.do()

    self.mySketch.setCoincident(SketchAPI_Point(self.myControlPoles[0]).coordinates(), aSpline.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.mySketch.setTangent(self.mySpline.result(), aSpline.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    #self.assertPointLineDistance(aSpline.poles().pnt(1), self.myControlLines[0])
    self.myExpectedFailure = True



if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    #assert model.checkPythonDump()

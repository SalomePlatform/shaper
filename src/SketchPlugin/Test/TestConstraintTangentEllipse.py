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
    Test constraint "Tangent" applied to ellipse and another entity
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-09-20"

class TestTangentEllipse(unittest.TestCase):
  def setUp(self):
    axisStart = GeomAPI_Pnt2d(20., 60.)
    axisEnd = GeomAPI_Pnt2d(80., 50.)
    passedPoint = GeomAPI_Pnt2d(60., 70.)

    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    macroEllipse = self.mySketch.addEllipse(axisStart, axisEnd, passedPoint, False)
    model.do()
    self.myEllipse = SketchAPI_Ellipse(model.lastSubFeature(self.mySketch, "SketchEllipse"))
    self.myCenter = macroEllipse.center()
    self.myFocus1 = macroEllipse.focus1()
    self.myFocus2 = macroEllipse.focus2()
    self.myMajorAxis = macroEllipse.majorAxis()
    self.myMajorStart = macroEllipse.majorAxisStart()
    self.myMajorEnd = macroEllipse.majorAxisEnd()
    self.myMinorAxis = macroEllipse.minorAxis()
    self.myMinorStart = macroEllipse.minorAxisStart()
    self.myMinorEnd = macroEllipse.minorAxisEnd()

    self.myDOF = 5
    self.myNbPoints = 7
    self.myNbLines = 2
    self.myNbArcs = 0
    self.myNbCircles = 0
    self.myNbEllipses = 1
    self.myNbInternals = 11
    self.myNbCoincidence = 0
    self.myNbTangency = 0

  def tearDown(self):
    model.end()
    self.checkDOF()
    self.assertPoints(self.myCenter.coordinates(), self.myEllipse.center())
    self.assertPoints(self.myFocus1.coordinates(), self.myEllipse.firstFocus())
    self.assertPoints(self.myFocus2.coordinates(), self.myEllipse.secondFocus())
    self.assertPoints(self.myMajorStart.coordinates(), self.myEllipse.majorAxisNegative())
    self.assertPoints(self.myMajorEnd.coordinates(), self.myEllipse.majorAxisPositive())
    self.assertPoints(self.myMajorAxis.startPoint(), self.myEllipse.majorAxisNegative())
    self.assertPoints(self.myMajorAxis.endPoint(), self.myEllipse.majorAxisPositive())
    self.assertPoints(self.myMinorStart.coordinates(), self.myEllipse.minorAxisNegative())
    self.assertPoints(self.myMinorEnd.coordinates(), self.myEllipse.minorAxisPositive())
    self.assertPoints(self.myMinorAxis.startPoint(), self.myEllipse.minorAxisNegative())
    self.assertPoints(self.myMinorAxis.endPoint(), self.myEllipse.minorAxisPositive())
    model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
    model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
    model.testNbSubFeatures(self.mySketch, "SketchArc", self.myNbArcs)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", self.myNbCircles)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", self.myNbEllipses)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", self.myNbInternals)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", self.myNbCoincidence)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintTangent", self.myNbTangency)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

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

  def assertTangentCircleEllipse(self, theCircle, theEllipse):
    axis = GeomAPI_Dir2d(theEllipse.firstFocus().x() - theEllipse.center().x(), theEllipse.firstFocus().y() - theEllipse.center().y())
    anEllipse = GeomAPI_Ellipse2d(theEllipse.center().pnt(), axis, theEllipse.majorRadius().value(), theEllipse.minorRadius().value())
    aCircle = GeomAPI_Circ2d(theCircle.center().pnt(), GeomAPI_Dir2d(1, 0), theCircle.radius().value())

    pOnE = GeomAPI_Pnt2d(0, 0)
    pOnC = GeomAPI_Pnt2d(0, 0)
    anEllipse.distance(aCircle, pOnE, pOnC)
    self.assertAlmostEqual(model.distancePointPoint(pOnE, theCircle.center()), theCircle.radius().value())

    dist1 = model.distancePointPoint(pOnC, theEllipse.firstFocus())
    dist2 = model.distancePointPoint(pOnC, theEllipse.secondFocus())
    self.assertAlmostEqual(dist1 + dist2, 2 * theEllipse.majorRadius().value())

  def assertTangentEllipses(self, theEllipse1, theEllipse2):
    axis1 = GeomAPI_Dir2d(theEllipse1.firstFocus().x() - theEllipse1.center().x(), theEllipse1.firstFocus().y() - theEllipse1.center().y())
    anEllipse1 = GeomAPI_Ellipse2d(theEllipse1.center().pnt(), axis1, theEllipse1.majorRadius().value(), theEllipse1.minorRadius().value())
    axis2 = GeomAPI_Dir2d(theEllipse2.firstFocus().x() - theEllipse2.center().x(), theEllipse2.firstFocus().y() - theEllipse2.center().y())
    anEllipse2 = GeomAPI_Ellipse2d(theEllipse2.center().pnt(), axis2, theEllipse2.majorRadius().value(), theEllipse2.minorRadius().value())

    p1 = GeomAPI_Pnt2d(0, 0)
    p2 = GeomAPI_Pnt2d(0, 0)
    anEllipse1.distance(anEllipse2, p1, p2)

    dist1 = model.distancePointPoint(p2, theEllipse1.firstFocus())
    dist2 = model.distancePointPoint(p2, theEllipse1.secondFocus())
    self.assertAlmostEqual(dist1 + dist2, 2 * theEllipse1.majorRadius().value())

    dist1 = model.distancePointPoint(p1, theEllipse2.firstFocus())
    dist2 = model.distancePointPoint(p1, theEllipse2.secondFocus())
    self.assertAlmostEqual(dist1 + dist2, 2 * theEllipse2.majorRadius().value())

  def assertPoints(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x())
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y())


  def test_line_tangent(self):
    """ Test 1. Set tangency between ellipse and a line
    """
    aLine = self.mySketch.addLine(10, 10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipse)


  def test_line_coincident_then_tangent(self):
    """ Test 2. Set tangency between ellipse and a line, if the extremity of the line is coincident with the ellipse
    """
    aLine = self.mySketch.addLine(10, 10, 90, 40)
    self.mySketch.setCoincident(aLine.endPoint(), self.myEllipse.result())
    self.myNbLines += 1
    self.myNbCoincidence += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipse)


  def test_line_tangent_then_coincident(self):
    """ Test 3. Set tangency between ellipse and a line, after that apply coincidence of extremity of the line and the ellipse's curve
    """
    aLine = self.mySketch.addLine(10, 10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setCoincident(aLine.startPoint(), self.myEllipse.result())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipse)


  def test_line_tangent_then_remove_coincidence(self):
    """ Test 4. Set tangency between ellipse and a line, which have a coincident point, then remove this coincidence
    """
    aLine = self.mySketch.addLine(10, 10, 90, 40)
    aCoincidence = self.mySketch.setCoincident(aLine.endPoint(), self.myEllipse.result())
    self.myNbLines += 1
    self.myNbCoincidence += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.myDocument.removeFeature(aCoincidence.feature())
    self.myNbCoincidence -= 1
    self.myDOF += 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipse)


  def test_circle_tangent(self):
    """ Test 5. Set tangency between ellipse and a circle
    """
    aCircle = self.mySketch.addCircle(30, 10, 20)
    self.myNbCircles += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), aCircle.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(aCircle, self.myEllipse)


  def test_circle_coincident_then_tangent(self):
    """ Test 6. Set tangency between ellipse and a circle, if the circle is coincident with start point of ellipse's minor axis
    """
    aCircle = self.mySketch.addCircle(30, 10, 20)
    self.mySketch.setCoincident(self.myMinorStart.coordinates(), aCircle.defaultResult())
    self.myNbCircles += 1
    self.myNbCoincidence += 1
    self.myDOF += 2
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), aCircle.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(aCircle, self.myEllipse)
    self.assertAlmostEqual(model.distancePointPoint(aCircle.center(), self.myMinorStart.coordinates()), aCircle.radius().value())


  def test_arc_tangent(self):
    """ Test 7. Set tangency between ellipse and a circular arc
    """
    anArc = self.mySketch.addArc(30, 10, 20, 10, 40, 10, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipse)


  def test_arc_coincident_then_tangent(self):
    """ Test 8. Set tangency between ellipse and an arc, if the extremity of the arc is coincident with the ellipse
    """
    anArc = self.mySketch.addArc(30, 10, 20, 10, 40, 10, False)
    self.mySketch.setCoincident(anArc.endPoint(), self.myEllipse.result())
    self.myNbArcs += 1
    self.myNbCoincidence += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipse)


  def test_arc_tangent_then_coincident(self):
    """ Test 9. Set tangency between ellipse and an arc, after that apply coincidence of extremity of the arc and the ellipse's curve
    """
    anArc = self.mySketch.addArc(30, 10, 20, 10, 40, 10, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setCoincident(anArc.startPoint(), self.myEllipse.result())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipse)


  def test_arc_tangent_then_remove_coincidence(self):
    """ Test 10. Set tangency between ellipse and an arc, which have a coincident point, then remove this coincidence
    """
    anArc = self.mySketch.addArc(30, 10, 20, 10, 40, 10, False)
    aCoincidence = self.mySketch.setCoincident(anArc.endPoint(), self.myEllipse.result())
    self.myNbArcs += 1
    self.myNbCoincidence += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.myDocument.removeFeature(aCoincidence.feature())
    self.myNbCoincidence -= 1
    self.myDOF += 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipse)


  def test_ellipse_tangent(self):
    """ Test 11. Set tangency between two ellipses
    """
    anEllipse = self.mySketch.addEllipse(10, 10, 20, -50, 20)
    self.myNbEllipses += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipse.result(), anEllipse.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentEllipses(anEllipse, self.myEllipse)



if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
    assert model.checkPythonDump()

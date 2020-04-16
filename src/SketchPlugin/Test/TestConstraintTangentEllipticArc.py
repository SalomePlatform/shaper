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
    Test constraint "Tangent" applied to elliptic arc and another entity
"""

import unittest
import math

from salome.shaper import model

from GeomAPI import *
from SketchAPI import *

__updated__ = "2019-10-04"

class TestTangentEllipticArc(unittest.TestCase):
  def setUp(self):
    center = GeomAPI_Pnt2d(30., 20.)
    axisEnd = GeomAPI_Pnt2d(50., 30.)
    startPoint = GeomAPI_Pnt2d(45, 40)
    endPoint = GeomAPI_Pnt2d(5, 6.11485435)

    model.begin()
    self.myDocument = model.moduleDocument()
    self.mySketch = model.addSketch(self.myDocument, model.defaultPlane("XOY"))
    macroEllipticArc = self.mySketch.addEllipticArc(center, axisEnd, startPoint, endPoint, False)
    model.do()
    self.myEllipticArc = SketchAPI_EllipticArc(model.lastSubFeature(self.mySketch, "SketchEllipticArc"))
    self.myCenter = macroEllipticArc.center()
    self.myFocus1 = macroEllipticArc.focus1()
    self.myFocus2 = macroEllipticArc.focus2()
    self.myMajorAxis = macroEllipticArc.majorAxis()
    self.myMajorStart = macroEllipticArc.majorAxisStart()
    self.myMajorEnd = macroEllipticArc.majorAxisEnd()
    self.myMinorAxis = macroEllipticArc.minorAxis()
    self.myMinorStart = macroEllipticArc.minorAxisStart()
    self.myMinorEnd = macroEllipticArc.minorAxisEnd()

    self.myDOF = 7
    self.myNbPoints = 7
    self.myNbLines = 2
    self.myNbArcs = 0
    self.myNbCircles = 0
    self.myNbEllipses = 0
    self.myNbEllipticArcs = 1
    self.myNbInternals = 11
    self.myNbCoincidence = 0
    self.myNbTangency = 0

  def tearDown(self):
    model.end()
    self.checkDOF()
    self.assertPoints(self.myCenter.coordinates(), self.myEllipticArc.center())
    self.assertPoints(self.myFocus1.coordinates(), self.myEllipticArc.firstFocus())
    self.assertPoints(self.myFocus2.coordinates(), self.myEllipticArc.secondFocus())
    self.assertPoints(self.myMajorStart.coordinates(), self.myEllipticArc.majorAxisNegative())
    self.assertPoints(self.myMajorEnd.coordinates(), self.myEllipticArc.majorAxisPositive())
    self.assertPoints(self.myMajorAxis.startPoint(), self.myEllipticArc.majorAxisNegative())
    self.assertPoints(self.myMajorAxis.endPoint(), self.myEllipticArc.majorAxisPositive())
    self.assertPoints(self.myMinorStart.coordinates(), self.myEllipticArc.minorAxisNegative())
    self.assertPoints(self.myMinorEnd.coordinates(), self.myEllipticArc.minorAxisPositive())
    self.assertPoints(self.myMinorAxis.startPoint(), self.myEllipticArc.minorAxisNegative())
    self.assertPoints(self.myMinorAxis.endPoint(), self.myEllipticArc.minorAxisPositive())
    model.testNbSubFeatures(self.mySketch, "SketchPoint", self.myNbPoints)
    model.testNbSubFeatures(self.mySketch, "SketchLine", self.myNbLines)
    model.testNbSubFeatures(self.mySketch, "SketchArc", self.myNbArcs)
    model.testNbSubFeatures(self.mySketch, "SketchCircle", self.myNbCircles)
    model.testNbSubFeatures(self.mySketch, "SketchEllipse", self.myNbEllipses)
    model.testNbSubFeatures(self.mySketch, "SketchEllipticArc", self.myNbEllipticArcs)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidenceInternal", self.myNbInternals)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintCoincidence", self.myNbCoincidence)
    model.testNbSubFeatures(self.mySketch, "SketchConstraintTangent", self.myNbTangency)


  def checkDOF(self):
    self.assertEqual(model.dof(self.mySketch), self.myDOF)

  def assertTangentLineEllipse(self, theLine, theEllipticArc):
    aLine = GeomAPI_Lin2d(theLine.startPoint().pnt(), theLine.endPoint().pnt())
    projF1 = aLine.project(theEllipticArc.firstFocus().pnt())
    projF2 = aLine.project(theEllipticArc.secondFocus().pnt())

    distF1P1 = model.distancePointPoint(theEllipticArc.firstFocus(), projF1)
    distF2P2 = model.distancePointPoint(theEllipticArc.secondFocus(), projF2)

    tgPoint = GeomAPI_Pnt2d((projF1.x() * distF2P2 + projF2.x() * distF1P1) / (distF1P1 + distF2P2), (projF1.y() * distF2P2 + projF2.y() * distF1P1) / (distF1P1 + distF2P2))
    distF1T = model.distancePointPoint(theEllipticArc.firstFocus(), tgPoint)
    distF2T = model.distancePointPoint(theEllipticArc.secondFocus(), tgPoint)
    NB_DIGITS = 7 - math.floor(math.log10(theEllipticArc.majorRadius().value()))
    self.assertAlmostEqual(distF1T + distF2T, 2 * theEllipticArc.majorRadius().value(), NB_DIGITS)

  def assertTangentCircleEllipse(self, theCircle, theEllipticArc):
    axis = GeomAPI_Dir2d(theEllipticArc.firstFocus().x() - theEllipticArc.center().x(), theEllipticArc.firstFocus().y() - theEllipticArc.center().y())
    anEllipticArc = GeomAPI_Ellipse2d(theEllipticArc.center().pnt(), axis, theEllipticArc.majorRadius().value(), theEllipticArc.minorRadius().value())
    aCircle = GeomAPI_Circ2d(theCircle.center().pnt(), GeomAPI_Dir2d(1, 0), theCircle.radius().value())

    pOnE = GeomAPI_Pnt2d(0, 0)
    pOnC = GeomAPI_Pnt2d(0, 0)
    anEllipticArc.distance(aCircle, pOnE, pOnC)
    self.assertAlmostEqual(model.distancePointPoint(pOnE, theCircle.center()), theCircle.radius().value())

    dist1 = model.distancePointPoint(pOnC, theEllipticArc.firstFocus())
    dist2 = model.distancePointPoint(pOnC, theEllipticArc.secondFocus())
    NB_DIGITS = 7 - math.floor(math.log10(theEllipticArc.majorRadius().value()))
    self.assertAlmostEqual(dist1 + dist2, 2 * theEllipticArc.majorRadius().value(), NB_DIGITS)

  def assertTangentEllipses(self, theEllipticArc1, theEllipticArc2):
    axis1 = GeomAPI_Dir2d(theEllipticArc1.firstFocus().x() - theEllipticArc1.center().x(), theEllipticArc1.firstFocus().y() - theEllipticArc1.center().y())
    anEllipticArc1 = GeomAPI_Ellipse2d(theEllipticArc1.center().pnt(), axis1, theEllipticArc1.majorRadius().value(), theEllipticArc1.minorRadius().value())
    axis2 = GeomAPI_Dir2d(theEllipticArc2.firstFocus().x() - theEllipticArc2.center().x(), theEllipticArc2.firstFocus().y() - theEllipticArc2.center().y())
    anEllipticArc2 = GeomAPI_Ellipse2d(theEllipticArc2.center().pnt(), axis2, theEllipticArc2.majorRadius().value(), theEllipticArc2.minorRadius().value())

    p1 = GeomAPI_Pnt2d(0, 0)
    p2 = GeomAPI_Pnt2d(0, 0)
    anEllipticArc1.distance(anEllipticArc2, p1, p2)

    dist1 = model.distancePointPoint(p2, theEllipticArc1.firstFocus())
    dist2 = model.distancePointPoint(p2, theEllipticArc1.secondFocus())
    NB_DIGITS = 7 - math.floor(math.log10(theEllipticArc1.majorRadius().value()))
    self.assertAlmostEqual(dist1 + dist2, 2 * theEllipticArc1.majorRadius().value())

    dist1 = model.distancePointPoint(p1, theEllipticArc2.firstFocus())
    dist2 = model.distancePointPoint(p1, theEllipticArc2.secondFocus())
    NB_DIGITS = 7 - math.floor(math.log10(theEllipticArc2.majorRadius().value()))
    self.assertAlmostEqual(dist1 + dist2, 2 * theEllipticArc2.majorRadius().value(), NB_DIGITS)

  def assertPoints(self, thePoint1, thePoint2):
    self.assertAlmostEqual(thePoint1.x(), thePoint2.x(), 6)
    self.assertAlmostEqual(thePoint1.y(), thePoint2.y(), 6)


  def test_line_tangent(self):
    """ Test 1. Set tangency between elliptic arc and a line
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_line_coincident_then_tangent(self):
    """ Test 2. Set tangency between elliptic arc and a line, if the extremity of the line is coincident with the elliptic arc
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.mySketch.setCoincident(aLine.endPoint(), self.myEllipticArc.result())
    self.myNbLines += 1
    self.myNbCoincidence += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_line_tangent_then_coincident(self):
    """ Test 3. Set tangency between elliptic arc and a line, after that apply coincidence of extremity of the line and the elliptic arc's curve
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(aLine.result(), self.myEllipticArc.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setCoincident(aLine.startPoint(), self.myEllipticArc.result())
    self.myNbCoincidence += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_line_tangent_then_remove_coincidence(self):
    """ Test 4. Set tangency between elliptic arc and a line, which have a coincident point, then remove this coincidence
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    aCoincidence = self.mySketch.setCoincident(aLine.endPoint(), self.myEllipticArc.result())
    self.myNbLines += 1
    self.myNbCoincidence += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.myDocument.removeFeature(aCoincidence.feature())
    self.myNbCoincidence -= 1
    self.myDOF += 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_line_coincident_point_then_tangent(self):
    """ Test 5. Set tangency between elliptic arc and a line, if the extremity of the line is coincident with the start point of elliptic arc
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.mySketch.setCoincident(aLine.endPoint(), self.myEllipticArc.startPoint())
    self.myNbLines += 1
    self.myNbCoincidence += 1
    self.myDOF += 2
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_line_tangent_then_coincident_point(self):
    """ Test 6. Set tangency between elliptic arc and a line, after that apply coincidence of extremities of the line and the elliptic arc
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    self.myNbLines += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)

    self.mySketch.setCoincident(aLine.startPoint(), self.myEllipticArc.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_line_tangent_then_remove_coincidence_on_extremity(self):
    """ Test 7. Set tangency between elliptic arc and a line, which have a coincident boundary point, then remove this coincidence
    """
    aLine = self.mySketch.addLine(10, -10, 90, 40)
    aCoincidence = self.mySketch.setCoincident(aLine.endPoint(), self.myEllipticArc.endPoint())
    self.myNbLines += 1
    self.myNbCoincidence += 1
    self.myDOF += 2
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aLine.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.myDocument.removeFeature(aCoincidence.feature())
    self.myNbCoincidence -= 1
    self.myDOF += 2
    model.do()

    self.assertTangentLineEllipse(aLine, self.myEllipticArc)


  def test_circle_tangent(self):
    """ Test 8. Set tangency between elliptic arc and a circle
    """
    aCircle = self.mySketch.addCircle(30, 10, 20)
    self.myNbCircles += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aCircle.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(aCircle, self.myEllipticArc)


  def test_circle_coincident_then_tangent(self):
    """ Test 9. Set tangency between elliptic arc and a circle, if the circle is coincident with start point of elliptic arc's minor axis
    """
    aCircle = self.mySketch.addCircle(30, 10, 20)
    self.mySketch.setCoincident(self.myEllipticArc.startPoint(), aCircle.defaultResult())
    self.myNbCircles += 1
    self.myNbCoincidence += 1
    self.myDOF += 2
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), aCircle.defaultResult())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(aCircle, self.myEllipticArc)
    self.assertAlmostEqual(model.distancePointPoint(aCircle.center(), self.myEllipticArc.startPoint()), aCircle.radius().value())


  def test_arc_tangent(self):
    """ Test 10. Set tangency between elliptic arc and a circular arc
    """
    anArc = self.mySketch.addArc(30, -10, 40, -10, 20, -10, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipticArc)


  def test_arc_coincident_then_tangent(self):
    """ Test 11. Set tangency between elliptic arc and an arc, if the extremities of the arcs are coincident
    """
    anArc = self.mySketch.addArc(30, -10, 40, -10, 20, -10, False)
    self.mySketch.setCoincident(anArc.endPoint(), self.myEllipticArc.startPoint())
    self.myNbArcs += 1
    self.myNbCoincidence += 1
    self.myDOF += 3
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipticArc)


  def test_arc_tangent_then_coincident(self):
    """ Test 12. Set tangency between elliptic arc and an arc, after that apply coincidence of extremities of the arcs
    """
    anArc = self.mySketch.addArc(30, -10, 40, -10, 20, -10, False)
    self.myNbArcs += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.mySketch.setCoincident(anArc.startPoint(), self.myEllipticArc.startPoint())
    self.myNbCoincidence += 1
    self.myDOF -= 2
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipticArc)


  def test_arc_tangent_then_remove_coincidence(self):
    """ Test 13. Set tangency between elliptic arc and an arc, which have a coincident point, then remove this coincidence
    """
    anArc = self.mySketch.addArc(30, -10, 40, -10, 20, -10, False)
    aCoincidence = self.mySketch.setCoincident(anArc.endPoint(), self.myEllipticArc.endPoint())
    self.myNbArcs += 1
    self.myNbCoincidence += 1
    self.myDOF += 4
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anArc.results()[-1])
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.myDocument.removeFeature(aCoincidence.feature())
    self.myNbCoincidence -= 1
    self.myDOF += 1
    model.do()

    self.assertTangentCircleEllipse(anArc, self.myEllipticArc)


  def test_ellipse_tangent(self):
    """ Test 14. Set tangency between ellipse and elliptic arc
    """
    anEllipse = self.mySketch.addEllipse(-30, 10, -10, 0, 20)
    self.myNbEllipses += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anEllipse.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentEllipses(anEllipse, self.myEllipticArc)


  def test_elliptic_arcs_tangent(self):
    """ Test 15. Set tangency between two elliptic arcs
    """
    anEllipticArc = self.mySketch.addEllipticArc(35, 20, 60, 30, 40, 40, 20, -0.4890968089561491, True)
    self.myNbEllipticArcs += 1
    self.myDOF += 7
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anEllipticArc.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentEllipses(anEllipticArc, self.myEllipticArc)


  def test_elliptic_arcs_coincident_then_tangent(self):
    """ Test 16. Set tangency between two elliptic arcs, if their extremities are coincident
    """
    anEllipticArc = self.mySketch.addEllipticArc(35, 20, 60, 30, 40, 40, 20, -0.4890968089561491, True)
    self.mySketch.setCoincident(anEllipticArc.startPoint(), self.myEllipticArc.startPoint())
    self.myNbEllipticArcs += 1
    self.myNbCoincidence += 1
    self.myDOF += 5
    model.do()

    self.mySketch.setTangent(self.myEllipticArc.result(), anEllipticArc.result())
    self.myNbTangency += 1
    self.myDOF -= 1
    model.do()

    self.assertTangentEllipses(anEllipticArc, self.myEllipticArc)



if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"

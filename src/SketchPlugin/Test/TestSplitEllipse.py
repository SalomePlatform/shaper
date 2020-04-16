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

from salome.shaper import model
from salome.shaper import geom
import math

from ModelAPI import *
from SketchAPI import *

CENTER = geom.Pnt2d(10, 10)
MAJOR_RADIUS = 50
MINOR_RADIUS = 30

DOF = 11
NB_LINES = 3
NB_ELLIPSES = 1
NB_ELLIPTIC_ARCS = 0
NB_COINCIDENCES = 5

TOLERANCE = 1.e-6

def checkFeaturesQuantity(theSketch):
    model.testNbSubFeatures(theSketch, "SketchLine", NB_LINES)
    model.testNbSubFeatures(theSketch, "SketchEllipse", NB_ELLIPSES)
    model.testNbSubFeatures(theSketch, "SketchEllipticArc", NB_ELLIPTIC_ARCS)
    model.testNbSubFeatures(theSketch, "SketchConstraintCoincidence", NB_COINCIDENCES)
    assert(model.dof(theSketch) == DOF)

def checkEllipticArcs(theSketch):
    for aSub in theSketch.features().list():
        aFeature = ModelAPI_Feature.feature(aSub)
        if aFeature is not None and aFeature.getKind() == "SketchEllipticArc":
            assertEllipticArc(SketchAPI_EllipticArc(aFeature))

def assertEllipticArc(theArc):
    assertPoints(theArc.center(), CENTER)
    assertPoints(theArc.majorAxisPositive(), geom.Pnt2d(CENTER.x() + MAJOR_RADIUS, CENTER.y()))
    assertPoints(theArc.minorAxisPositive(), geom.Pnt2d(CENTER.x(), CENTER.y() + MINOR_RADIUS))

def assertPoints(thePoint1, thePoint2):
  assert(math.fabs(thePoint1.x() - thePoint2.x()) < TOLERANCE), "{} != {}".format(thePoint1.x(), thePoint2.x())
  assert(math.fabs(thePoint1.y() - thePoint2.y()) < TOLERANCE), "{} != {}".format(thePoint1.y(), thePoint2.y())


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchEllipse_1 = Sketch_1.addEllipse(CENTER.x(), CENTER.y(), CENTER.x() + math.sqrt(MAJOR_RADIUS**2 - MINOR_RADIUS**2), CENTER.y(), MINOR_RADIUS)
SketchLine_1 = Sketch_1.addLine(-16.74176451428603, -15.34869012470842, -16.85909682653373, 35.30399198463829)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchEllipse_1.result())
SketchLine_2 = Sketch_1.addLine(-16.85909682653373, 35.30399198463829, 20.9032928583277, -19.27802168426675)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchEllipse_1.result())
SketchLine_3 = Sketch_1.addLine(34.69765676551338, 36.08465583643841, 35.0422024535432, -15.96612629290852)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchEllipse_1.result())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchEllipse_1.result())
model.do()

checkFeaturesQuantity(Sketch_1)
checkEllipticArcs(Sketch_1)

# split the ellipse
SketchSplit = Sketch_1.addSplit(SketchEllipse_1, geom.Pnt2d(CENTER.x() + MAJOR_RADIUS, CENTER.y()))
model.do()
NB_ELLIPSES -= 1
NB_ELLIPTIC_ARCS += 2
NB_COINCIDENCES += 4
DOF += 3

checkFeaturesQuantity(Sketch_1)
checkEllipticArcs(Sketch_1)

# split the middle arc of ellipse
EllipticArc = SketchAPI_EllipticArc(model.lastSubFeature(Sketch_1, "SketchEllipticArc"))
ANGLE = -math.pi/2 - math.pi/10
SketchSplit = Sketch_1.addSplit(EllipticArc, geom.Pnt2d(CENTER.x() + MAJOR_RADIUS * math.cos(ANGLE), CENTER.y() + MINOR_RADIUS * math.sin(ANGLE)))
model.do()
NB_ELLIPTIC_ARCS += 2
NB_COINCIDENCES += 4
DOF += 8

checkFeaturesQuantity(Sketch_1)
checkEllipticArcs(Sketch_1)

# try to split the boundary arc of ellipse,
# it shoult fail, because there is no coincident points
EllipticArc = SketchAPI_EllipticArc(model.lastSubFeature(Sketch_1, "SketchEllipticArc"))
SketchSplit = Sketch_1.addSplit(EllipticArc, geom.Pnt2d(CENTER.x() - MAJOR_RADIUS, CENTER.y()))
model.end()
aValidators = ModelAPI_Session.get().validators()
assert(not aValidators.validate(SketchSplit.feature()))

# remove previous split and add coincidence
model.undo()
model.begin()
Part_1_doc.removeFeature(SketchSplit.feature())
model.do()
Sketch_1.setCoincident(SketchLine_1.endPoint(), EllipticArc.result())
Sketch_1.setCoincident(SketchLine_2.startPoint(), EllipticArc.result())
model.do()
NB_COINCIDENCES += 1
DOF -= 1

# split the boundary arc of ellipse
SketchSplit = Sketch_1.addSplit(EllipticArc, geom.Pnt2d(CENTER.x() - MAJOR_RADIUS, CENTER.y()))
model.do()
NB_ELLIPTIC_ARCS += 1
NB_COINCIDENCES += 4
DOF += 4

checkFeaturesQuantity(Sketch_1)
checkEllipticArcs(Sketch_1)

model.end()

assert(model.checkPythonDump())

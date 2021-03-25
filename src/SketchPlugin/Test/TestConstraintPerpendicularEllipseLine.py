# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
import math
from GeomAPI import *

TOLERANCE = 1.e-7

def checkPerpendicular(theLine, theEllipse):
    focus1 = theEllipse.firstFocus()
    focus2 = theEllipse.secondFocus()
    pf1 = theLine.project(focus1)
    pf2 = theLine.project(focus2)
    dist1 = focus1.distance(pf1)
    dist2 = focus2.distance(pf2)
    if math.fabs(dist1 - dist2) < TOLERANCE:
        # no need further check, the line is equal to one of the ellipses axis
        return
    x = (dist2 * pf1.x() - dist1 * pf2.x()) / (dist2 - dist1)
    y = (dist2 * pf1.y() - dist1 * pf2.y()) / (dist2 - dist1)
    pnt = GeomAPI_Pnt(x, y, 0)
    # check point on ellipse
    majorRad = theEllipse.majorRadius()
    assert(math.fabs(pnt.distance(focus1) + pnt.distance(focus2) - 2.0 * majorRad) < TOLERANCE * majorRad)


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-40, 10, -20, 30)
SketchArc_1 = Sketch_1.addArc(25, -30, 42, -34, 13, -17.31142245955048, False)
SketchEllipse_1 = Sketch_1.addEllipse(-50, 0, -30, -5, 15)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_2, SketchLine_3] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchEllipticArc_1 = Sketch_1.addEllipticArc(5, 30, -5, 45, -7, 47, 15, 33.68010611, False)
[SketchPoint_8, SketchPoint_9, SketchPoint_10, SketchPoint_11, SketchPoint_12, SketchPoint_13, SketchPoint_14, SketchLine_4, SketchLine_5] = SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()

# check error on perpendicularity of arc and elliptic arc
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchArc_1.results()[1], SketchEllipticArc_1.result())
model.do()
assert(SketchConstraintPerpendicular_1.feature().error() != "")

# avoid the failure
Part_1_doc.removeFeature(SketchConstraintPerpendicular_1.feature())
model.do()
assert(Sketch_1.feature().error() == "")

# set correct constraints
SketchConstraintPerpendicular_2 = Sketch_1.setPerpendicular(SketchEllipticArc_1.result(), SketchLine_1.result())
SketchConstraintPerpendicular_3 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchEllipse_1.result())
model.do()

checkPerpendicular(SketchLine_1.defaultResult().shape().edge().line(), SketchEllipse_1.defaultResult().shape().edge().ellipse())
checkPerpendicular(SketchLine_1.defaultResult().shape().edge().line(), SketchEllipticArc_1.defaultResult().shape().edge().ellipse())
model.end()

assert(model.checkPythonDump())

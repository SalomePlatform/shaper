# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Cylinder_1_1/Face_2"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Cylinder_1_1/Face_1][Cylinder_1_1/Face_3]"), True)
SketchCircle_1 = SketchProjection_1.createdFeature()
SketchLine_1 = Sketch_1.addLine(-0.2715703110882526, -4.992619509449466, -1.152966889672805, 4.865251005995354)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
model.do()

SketchSplit_1 = Sketch_1.addSplit(SketchAPI_Circle(SketchCircle_1), geom.Pnt2d(-5, 0))
model.end()

# check Split is invalid
assert(SketchSplit_1.feature().error() != "")

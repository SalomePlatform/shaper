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

# Test move the group in history for selection of a whole result

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(35.07635467980296, 23.01108374384237, -25.74753694581282, 23.01108374384237)
SketchLine_2 = Sketch_1.addLine(-25.74753694581282, 23.01108374384237, -25.74753694581282, -23.13546798029557)
SketchLine_3 = Sketch_1.addLine(-25.74753694581282, -23.13546798029557, 35.07635467980296, -23.13546798029557)
SketchLine_4 = Sketch_1.addLine(35.07635467980296, -23.13546798029557, 35.07635467980296, 23.01108374384237)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 80, 0)
# selection of a whole result: 6 faces of a box
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("SOLID", "Extrusion_1_1")])
model.testNbSubShapes(Group_1, GeomAPI_Shape.FACE, [6])
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 10, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"))
SketchCircle_1 = Sketch_2.addCircle(-7.736745115674536, 52.14422040986419, 6.760003867274182)
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.do()
# move group after the extrusion-cut, so, it refers to it
Part_1_doc.moveFeature(Group_1.feature(), ExtrusionCut_1.feature())
model.end()

# check that there is a hole appeared in the group-results
assert(len(Group_1.feature().results())==1)
model.testNbSubShapes(Group_1, GeomAPI_Shape.FACE, [8])

assert(model.checkPythonDump())

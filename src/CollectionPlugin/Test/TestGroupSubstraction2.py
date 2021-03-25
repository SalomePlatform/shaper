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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-30, -17.32050807568877, 30, -17.32050807568877)
SketchLine_2 = Sketch_1.addLine(30, -17.32050807568877, 0, 34.64101615137755)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(0, 34.64101615137755, -30, -17.32050807568877)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchLine_4 = Sketch_1.addLine(30, -17.32050807568877, 0, 0)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(0, 0, 0, 34.64101615137755)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(-30, -17.32050807568877, 0, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.endPoint())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_5.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_4.result(), SketchLine_6.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 60)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Group_1_objects = [model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Extrusion_1_1_3/To_Face"), model.selection("FACE", "Extrusion_1_1_3/From_Face"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1_2/To_Face"), model.selection("FACE", "Extrusion_1_1_2/From_Face"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_5")]
Group_1 = model.addGroup(Part_1_doc, "Faces", Group_1_objects)
Group_2 = model.addGroup(Part_1_doc, "Faces", [model.selection("SOLID", "Extrusion_1_1_2")])
GroupSubstraction_1 = model.addGroupSubstraction(Part_1_doc, [model.selection("COMPOUND", "Group_1")], [model.selection("COMPOUND", "Group_2")])
model.end()

from GeomAPI import *

model.testNbResults(GroupSubstraction_1, 1)
model.testNbSubResults(GroupSubstraction_1, [0])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(GroupSubstraction_1, GeomAPI_Shape.VERTEX, [48])

assert(model.checkPythonDump())

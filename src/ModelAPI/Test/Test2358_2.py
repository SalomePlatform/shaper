# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "h", "100")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 50)
SketchLine_2 = Sketch_1.addLine(0, 50, 50, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(50, 50, 50, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(50, 0, 0, 0)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchLine_1.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 50)
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintParallel_2 = Sketch_1.setParallel(SketchLine_2.result(), SketchLine_4.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_5 = Sketch_2.addLine(10, 21.9014765990379, 0, 21.9014765990379)
SketchLine_5.setName("SketchLine_9")
SketchLine_5.result().setName("SketchLine_9")
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_6 = SketchProjection_1.createdFeature()
SketchLine_6.setName("SketchLine_10")
SketchLine_6.result().setName("SketchLine_10")
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.result())
SketchLine_7 = Sketch_2.addLine(0, 21.9014765990379, 0, 31.9014765990379)
SketchLine_7.setName("SketchLine_11")
SketchLine_7.result().setName("SketchLine_11")
SketchLine_8 = Sketch_2.addLine(0, 31.9014765990379, 10, 31.9014765990379)
SketchLine_8.setName("SketchLine_12")
SketchLine_8.result().setName("SketchLine_12")
SketchLine_9 = Sketch_2.addLine(10, 31.9014765990379, 10, 21.9014765990379)
SketchLine_9.setName("SketchLine_13")
SketchLine_9.result().setName("SketchLine_13")
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_9.result())
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_9.result(), 10)
SketchConstraintLength_4 = Sketch_2.setLength(SketchLine_8.result(), 10)
model.do()
Sketch_2.setName("Sketch_3")
Sketch_2.result().setName("Sketch_3")
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_3/Face-SketchLine_13r-SketchLine_12r-SketchLine_11r-SketchLine_9r_wire")], model.selection(), model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3"), 0, model.selection(), 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_10 = Sketch_3.addLine(29.60939424565112, 0, 19.60939424565112, 0)
SketchLine_10.setName("SketchLine_14")
SketchLine_10.result().setName("SketchLine_14")
SketchProjection_2 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_11 = SketchProjection_2.createdFeature()
SketchLine_11.setName("SketchLine_15")
SketchLine_11.result().setName("SketchLine_15")
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.result())
SketchLine_12 = Sketch_3.addLine(19.60939424565112, 0, 19.60939424565112, 5)
SketchLine_12.setName("SketchLine_16")
SketchLine_12.result().setName("SketchLine_16")
SketchLine_13 = Sketch_3.addLine(19.60939424565112, 5, 29.60939424565112, 5)
SketchLine_13.setName("SketchLine_17")
SketchLine_13.result().setName("SketchLine_17")
SketchLine_14 = Sketch_3.addLine(29.60939424565112, 5, 29.60939424565112, 0)
SketchLine_14.setName("SketchLine_18")
SketchLine_14.result().setName("SketchLine_18")
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_3 = Sketch_3.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_5 = Sketch_3.setVertical(SketchLine_14.result())
SketchConstraintLength_5 = Sketch_3.setLength(SketchLine_12.result(), 5)
SketchConstraintLength_6 = Sketch_3.setLength(SketchLine_13.result(), 10)
model.do()
Sketch_3.setName("Sketch_4")
Sketch_3.result().setName("Sketch_4")
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "Sketch_4")], model.selection(), "h", 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
Fillet_1_objects = [model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_12)(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face)]"), model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_12)(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face)][ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face]"), model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2]"), model.selection("EDGE", "[(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_9)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)][ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face][(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_9)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3]"), model.selection("EDGE", "[ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_12)(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face)]"), model.selection("EDGE", "[(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_9)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3]"), model.selection("EDGE", "[(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/From_Face)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_9)(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1]"), model.selection("EDGE", "[ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1][(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_12)(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face)]"), model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_12)(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face)][ExtrusionCut_2_1/Generated_Face&Sketch_4/SketchLine_16&weak_name_2]"), model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3)(ExtrusionCut_2_1/Modified_Face&Sketch_3/SketchLine_12)(ExtrusionCut_2_1/Modified_Face&Extrusion_1_1/To_Face)][ExtrusionCut_2_1/Generated_Face&Sketch_4/SketchLine_18&weak_name_2]")]
Fillet_1 = model.addFillet(Part_1_doc, Fillet_1_objects, 2)
Group_1_objects = [
model.selection("FACE", "(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_14)(Fillet_1_1/MF:Fillet_Face&Sketch_3/SketchLine_12)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_15)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_1)"),
model.selection("FACE", "(Fillet_1_1/MF:Fillet_Face&Sketch_3/SketchLine_12)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_11)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_16)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_2)"),
model.selection("FACE", "(Fillet_1_1/MF:Fillet_Face&Sketch_3/SketchLine_9)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_13)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_6)"),
model.selection("FACE", "(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_5)(Fillet_1_1/MF:Fillet_Face&Sketch_3/SketchLine_9)(Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_12)")
]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
Group_2 = model.addGroup(Part_1_doc, [model.selection("EDGE", "[Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_9][Fillet_1_1/MF:Fillet_Face&Extrusion_1_1/To_Face]"), model.selection("EDGE", "[Fillet_1_1/MF:Fillet_Face&Sketch_1/SketchLine_3][Fillet_1_1/GF:Fillet&Fillet_1_1/FilletSelected_9]")])
Group_3 = model.addGroup(Part_1_doc, [model.selection("VERTEX", "[Fillet_1_1/MF:Fillet_Face&Sketch_3/SketchLine_9][ExtrusionCut_1_1/Generated_Face&Sketch_3/SketchLine_13][Fillet_1_1/MF:Fillet_Face&Sketch_1/SketchLine_3]"), model.selection("VERTEX", "[ExtrusionCut_1_1/Generated_Face&Sketch_3/SketchLine_13][Fillet_1_1/MF:Fillet_Face&Sketch_1/SketchLine_3][Fillet_1_1/MF:Fillet_Face&Sketch_3/SketchLine_12]")])
model.do()

Folder_1 = model.addFolder(Part_1_doc, Sketch_3, ExtrusionCut_2)
Folder_1.setName("Box")
Folder_2 = model.addFolder(Part_1_doc, Group_1, Group_2)
Folder_2.setName("Groups")

from GeomAPI import GeomAPI_Shape

# check result shape
model.testNbResults(Fillet_1, 1)
model.testNbSubResults(Fillet_1, [0])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.FACE, [38])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.EDGE, [184])
model.testNbSubShapes(Fillet_1, GeomAPI_Shape.VERTEX, [368])
model.testResultsVolumes(Fillet_1, [117584.019523708775523118674755096])
# check groups
model.checkGroup(Group_1, GeomAPI_Shape.FACE)
model.checkGroup(Group_2, GeomAPI_Shape.EDGE)
model.checkGroup(Group_3, GeomAPI_Shape.VERTEX)

model.end()

assert(model.checkPythonDump())

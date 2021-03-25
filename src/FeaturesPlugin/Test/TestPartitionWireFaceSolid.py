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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Torus_1 = model.addTorus(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 15, 3)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(-6.378039652249388, 16.497623739925, 8.984305600666083, -24.54329369827008)
SketchLine_2 = Sketch_1.addLine(8.984305600666083, -24.54329369827008, 36.73713621773835, -5.929982231235201)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(36.73713621773835, -5.929982231235201, 19.18554250372658, 21.11854572580065)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(19.18554250372658, 21.11854572580065, -6.378039652249388, 16.497623739925)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchCircle_1 = Sketch_1.addCircle(14.49765103937751, 9.678507230150521, 3.831444106492329)
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchCircle_1_2")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_5 = Sketch_2.addLine(-15.11211565655763, 11.99814589971457, -14.79635576556081, -11.77064279967296)
SketchLine_6 = Sketch_2.addLine(-14.79635576556081, -11.77064279967296, -3.043616368139293, -11.46189011940212)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(-3.043616368139293, -11.46189011940212, -3.922197168000547, 15.83568871823551)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(-3.922197168000547, 15.83568871823551, -15.11211565655763, 11.99814589971457)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_8.endPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_8"), model.selection("EDGE", "Sketch_2/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_6"), model.selection("EDGE", "Sketch_2/SketchLine_7")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Partition_1_objects = [model.selection("FACE", "Face_1_1"), model.selection("SOLID", "Torus_1_1"), model.selection("WIRE", "Wire_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()

model.checkResult(Partition_1,model,1,[3],[1],[4],[22],[46])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

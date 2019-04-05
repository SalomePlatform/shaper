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
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_1/From_Face"), 25, True)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_5 = Sketch_2.addLine(84.67753244325934, -72.65003691291119, -28.95543815719345, -72.65003691291119)
SketchLine_6 = Sketch_2.addLine(-28.95543815719345, -72.65003691291119, -28.95543815719345, 10.01640181835304)
SketchLine_7 = Sketch_2.addLine(-28.95543815719345, 10.01640181835304, 84.67753244325934, 10.01640181835304)
SketchLine_8 = Sketch_2.addLine(84.67753244325934, 10.01640181835304, 84.67753244325934, -72.65003691291119)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_8.result())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_6"), model.selection("EDGE", "Sketch_2/SketchLine_8"), model.selection("EDGE", "Sketch_2/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_7")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Wire_1_1")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("FACE", "Face_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_9 = Sketch_3.addLine(0, 0, 0, 10)
SketchLine_10 = Sketch_3.addLine(0, 10, 10, 10)
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_3.addLine(10, 10, 10, 0)
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_3.addLine(10, 0, 0, 0)
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_12 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintRigid_2 = Sketch_3.setFixed(SketchLine_9.startPoint())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_9.result())
SketchConstraintLength_3 = Sketch_3.setLength(SketchLine_12.result(), 10)
SketchConstraintLength_4 = Sketch_3.setLength(SketchLine_9.result(), 10)
SketchConstraintParallel_3 = Sketch_3.setParallel(SketchLine_9.result(), SketchLine_11.result())
SketchConstraintParallel_4 = Sketch_3.setParallel(SketchLine_10.result(), SketchLine_12.result())
SketchConstraintPerpendicular_2 = Sketch_3.setPerpendicular(SketchLine_9.result(), SketchLine_12.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_12r-SketchLine_11r-SketchLine_10r-SketchLine_9r")], model.selection(), 50, 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1_1")], [model.selection("SOLID", "Extrusion_2_1")])
model.do()
model.end()

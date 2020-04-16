# Copyright (C) 2017-2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(-19.49406233194174, -69.37451616542793, -72.71146724766345, -69.37451616542793)
SketchLine_2 = Sketch_1.addLine(-72.71146724766345, -69.37451616542793, -72.71146724766345, -28.67067780808921)
SketchLine_3 = Sketch_1.addLine(-72.71146724766345, -28.67067780808921, -19.49406233194174, -28.67067780808921)
SketchLine_4 = Sketch_1.addLine(-19.49406233194174, -28.67067780808921, -19.49406233194174, -69.37451616542793)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(16.67888107989752, -75.92151145213565, 19.40636912075787, -34.81908596556777)
SketchPoint_1 = Sketch_1.addPoint(0.6128545881206371, -78.63973361497078)
SketchCircle_1 = Sketch_1.addCircle(51.0302184611028, -58.25492948210744, 14.6764214847306)
SketchCircle_2 = Sketch_1.addCircle(57.27434431552349, -55.3477547099222, 30.40870102508203)
model.do()
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "PartSet/Origin")])
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_5")])
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_2_2")])
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_6 = Sketch_2.addLine(73.69205241424749, 19.68979803478298, 29.86213888681927, 19.68979803478298)
SketchLine_7 = Sketch_2.addLine(29.86213888681927, 19.68979803478298, 29.86213888681927, 46.16063196348271)
SketchLine_8 = Sketch_2.addLine(29.86213888681927, 46.16063196348271, 73.69205241424749, 46.16063196348271)
SketchLine_9 = Sketch_2.addLine(73.69205241424749, 46.16063196348271, 73.69205241424749, 19.68979803478298)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_9.result())
model.do()
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r_wire")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "Sketch_2/SketchLine_6"), 120, 2)
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "AngularCopy_1_1_1"), model.selection("FACE", "AngularCopy_1_1_2")])
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OX"), 50)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_2_1/Front"), 5, True)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_2_1"), model.selection("FACE", "Plane_1")])
Compound_1_objects = [model.selection("VERTEX", "Sketch_1/SketchPoint_1"),
                      model.selection("EDGE", "Sketch_1/SketchLine_1"),
                      model.selection("WIRE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r_wire"),
                      model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r"),
                      model.selection("VERTEX", "Vertex_1_1"),
                      model.selection("EDGE", "Edge_1_1"),
                      model.selection("WIRE", "Wire_1_1"),
                      model.selection("FACE", "Face_1_1"),
                      model.selection("SHELL", "Shell_1_1"),
                      model.selection("SOLID", "Translation_1_1"),
                      model.selection("COMPSOLID", "Partition_1_1")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
model.do()

model.checkResult(Compound_1, model, 1, [11], [3], [22], [92], [186])

model.end()

assert(model.checkPythonDump())

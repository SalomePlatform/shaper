# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(306.7092651757188, 0, 0, 0)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 127.7955271565495)
SketchLine_3 = Sketch_1.addLine(0, 127.7955271565495, 306.7092651757188, 127.7955271565495)
SketchLine_4 = Sketch_1.addLine(306.7092651757188, 127.7955271565495, 306.7092651757188, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(0, 127.7955271565495, -349.8402555910542, 127.7955271565495)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-349.8402555910542, 127.7955271565495, -349.8402555910542, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(-349.8402555910542, 0, 0, 0)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_7.endPoint())
SketchLine_8 = Sketch_1.addLine(0, 0, 0, -164.5367412140575)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(0, -164.5367412140575, -349.8402555910542, -164.5367412140575)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(-349.8402555910542, -164.5367412140575, -349.8402555910542, 0)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_10.endPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_8.result())
SketchLine_11 = Sketch_1.addLine(527.1565495207667, -78.27476038338659, 81.46964856230041, -78.27476038338659)
SketchLine_12 = Sketch_1.addLine(81.46964856230041, -78.27476038338659, 81.46964856230041, -285.9424920127795)
SketchLine_13 = Sketch_1.addLine(81.46964856230041, -285.9424920127795, 527.1565495207667, -285.9424920127795)
SketchLine_14 = Sketch_1.addLine(527.1565495207667, -285.9424920127795, 527.1565495207667, -78.27476038338659)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_14.result())
model.do()
Extrusion_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchLine_2f-SketchLine_5f-SketchLine_6f-SketchLine_7f"), model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchLine_10r-SketchLine_9r-SketchLine_8r-SketchLine_7r"), model.selection("FACE", "Sketch_1/Face-SketchLine_11f-SketchLine_12f-SketchLine_13f-SketchLine_14f")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection(), 10, 0)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_1_2")])
model.do()
model.end()

assert(model.checkPythonDump())

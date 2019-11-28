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
SketchLine_1 = Sketch_1.addLine(-44.48110465116279, 12.33139534883721, -38.60901162790698, 48.15116279069767)
SketchLine_2 = Sketch_1.addLine(-38.60901162790698, 48.15116279069767, 16.88226744186047, 78.68604651162791)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(16.88226744186047, 78.68604651162791, 48.29796511627906, 10.86337209302326)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(48.29796511627906, 10.86337209302326, 31.85610465116278, -38.16860465116279)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(31.85610465116278, -38.16860465116279, -44.48110465116279, 12.33139534883721)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r"))
SketchLine_6 = Sketch_2.addLine(31.85610465116278, -38.16860465116279, -40.43276493886778, -56.03425756438362)
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_4_EndVertex"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.startPoint(), SketchPoint_1.result())
SketchLine_7 = Sketch_2.addLine(-40.43276493886778, -56.03425756438362, -115.5003601973775, -26.06900348999911)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(-115.5003601973775, -26.06900348999911, -106.2327558444751, 34.47934494896344)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_2.addLine(-106.2327558444751, 34.47934494896344, -44.48110465116279, 12.33139534883721)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_StartVertex"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchPoint_2.result())
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_5")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Face_2_objects = [model.selection("EDGE", "Sketch_2/SketchLine_9"), model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_6"), model.selection("EDGE", "Sketch_2/SketchLine_7"), model.selection("EDGE", "Sketch_2/SketchLine_8")]
Face_2 = model.addFace(Part_1_doc, Face_2_objects)
Partition_1_objects = [model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_1_1"), model.selection("FACE", "PartSet/YOZ")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Union_1_objects = [model.selection("FACE", "Partition_1_1_1"), model.selection("FACE", "Partition_1_1_3"), model.selection("FACE", "Partition_1_1_4")]
Union_1 = model.addFuse(Part_1_doc, Union_1_objects, True, keepSubResults = True)
model.do()

model.checkResult(Union_1,model,1,[2],[0],[2],[13],[26])
model.testHaveNamingSubshapes(Union_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

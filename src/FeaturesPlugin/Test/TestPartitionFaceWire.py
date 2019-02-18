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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(-27.59408640325028, 19.28272273466096, -69.22045455254333, 36.34312971236017, -20.39867310978187, 63.6903576534723, True)
SketchLine_1 = Sketch_1.addLine(-20.39867310978187, 63.6903576534723, 50.06261173489778, 52.27344552622073)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchLine_2 = Sketch_1.addLine(50.06261173489778, 52.27344552622073, 61.51017441860466, -50.20639534883721)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(61.51017441860466, -50.20639534883721, -40.37063953488371, -58.72093023255813)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-40.37063953488371, -58.72093023255813, -8.587370006050145, 0.9678324888285939)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-8.587370006050145, 0.9678324888285939, -69.22045455254333, 36.34312971236017)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_5.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_2 = Sketch_2.addArc(25.75044090453202, 23.87245760846999, -31.39978593836629, 46.7325483456293, 20.55274339063303, 85.20528827247816, True)
SketchLine_6 = Sketch_2.addLine(20.55274339063303, 85.20528827247816, 74.86874397491491, -32.65992894423484)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(74.86874397491491, -32.65992894423484, 9.995513504597909, -67.56790534016733)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(9.995513504597909, -67.56790534016733, -36.0335881148175, -7.328477046301525)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_2.addLine(-36.0335881148175, -7.328477046301525, -31.39978593836629, 46.7325483456293)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchArc_2.startPoint(), SketchLine_9.endPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchArc_1_2"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_5")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Face_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_9"), model.selection("EDGE", "Sketch_2/SketchArc_2_2"), model.selection("EDGE", "Sketch_2/SketchLine_6"), model.selection("EDGE", "Sketch_2/SketchLine_7"), model.selection("EDGE", "Sketch_2/SketchLine_8")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("WIRE", "Wire_1_1")])
model.do()

model.checkResult(Partition_1,model,1,[6],[0],[5],[39],[78])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

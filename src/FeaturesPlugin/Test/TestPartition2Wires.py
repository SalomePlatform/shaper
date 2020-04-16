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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(-59.3375, 27.49444444444443, -20.05972222222222, 71.26111111111109)
SketchLine_2 = Sketch_1.addLine(-20.05972222222222, 71.26111111111109, 31.5625, 60.31944444444444)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(31.5625, 60.31944444444444, 48.95694444444444, -41.52222222222221)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(48.95694444444444, -41.52222222222221, -7.434722222222216, -63.6861111111111)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-7.434722222222216, -63.6861111111111, -12.20416666666666, 3.0861111111111)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-12.20416666666666, 3.0861111111111, -59.3375, 27.49444444444443)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchArc_1 = Sketch_2.addArc(-25.27347239223771, 29.23826809458483, -58.42699662355402, 67.59234514649975, 1.894762451441686, -13.56451698932517, False)
SketchLine_7 = Sketch_2.addLine(-58.42699662355402, 67.59234514649975, 66.71130562210065, 43.53978835123107)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchArc_1.startPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(66.71130562210065, 43.53978835123107, 30.30743587791019, -67.62202818906478)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_2.addLine(30.30743587791019, -67.62202818906478, 1.894762451441686, -13.56451698932518)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchArc_1.endPoint(), SketchLine_9.endPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_7"), model.selection("EDGE", "Sketch_2/SketchLine_8"), model.selection("EDGE", "Sketch_2/SketchLine_9"), model.selection("EDGE", "Sketch_2/SketchArc_1_2")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Wire_2_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_5"), model.selection("EDGE", "Sketch_1/SketchLine_6")]
Wire_2 = model.addWire(Part_1_doc, Wire_2_objects)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("WIRE", "Wire_2_1"), model.selection("WIRE", "Wire_1_1")])
model.do()

model.checkResult(Partition_1,model,1,[2],[0],[0],[22],[44])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

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
SketchLine_1 = Sketch_1.addLine(64.44596912521442, 49.72041166380789, -52.66552315608919, 49.72041166380789)
SketchLine_2 = Sketch_1.addLine(-52.66552315608919, 49.72041166380789, -52.66552315608919, -67.3910806174957)
SketchLine_3 = Sketch_1.addLine(-52.66552315608919, -67.3910806174957, 64.44596912521442, -67.3910806174957)
SketchLine_4 = Sketch_1.addLine(64.44596912521442, -67.3910806174957, 64.44596912521442, 49.72041166380789)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(34.99485420240136, 11.60720411663807, -11.7804459691252, 11.60720411663807)
SketchLine_6 = Sketch_1.addLine(-11.7804459691252, 11.60720411663807, -11.7804459691252, -33.78216123499142)
SketchLine_7 = Sketch_1.addLine(-11.7804459691252, -33.78216123499142, 34.99485420240136, -33.78216123499142)
SketchLine_8 = Sketch_1.addLine(34.99485420240136, -33.78216123499142, 34.99485420240136, 11.60720411663807)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), 100, 100)
Face_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_3")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("FACE", "Face_1_1")])
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [13])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.EDGE, [56])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.VERTEX, [112])
model.testResultsVolumes(Partition_1, [424620.237783511402085423469543457])

assert(model.checkPythonDump())

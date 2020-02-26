# Copyright (C) 2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(52.9121725731895, 8.799859429739596, 7.781201848998454, 8.799859429739596)
SketchLine_2 = Sketch_1.addLine(7.781201848998454, 8.799859429739596, 7.781201848998454, 36.33111129445148)
SketchLine_3 = Sketch_1.addLine(7.781201848998454, 36.33111129445148, 52.9121725731895, 36.33111129445148)
SketchLine_4 = Sketch_1.addLine(52.9121725731895, 36.33111129445148, 52.9121725731895, 8.799859429739596)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(29.87981510015411, 36.33111129445148, 29.87981510015411, 8.799859429739596)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchCircle_1 = Sketch_1.addCircle(88.70570107858241, 30.96918335901386, 15.95582825781952)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 20, 0)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_6 = Sketch_2.addLine(91.78842679005474, 52.44197384283513, 18.42664179347724, 52.44197384283513)
SketchLine_7 = Sketch_2.addLine(18.42664179347724, 52.44197384283513, 18.42664179347724, 23.73518840939176)
SketchLine_8 = Sketch_2.addLine(18.42664179347724, 23.73518840939176, 91.78842679005474, 23.73518840939176)
SketchLine_9 = Sketch_2.addLine(91.78842679005474, 23.73518840939176, 91.78842679005474, 52.44197384283513)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_9.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_6r-SketchLine_7f-SketchLine_8f-SketchLine_9f")], model.selection(), 10, 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Compound_1_1_1_1")], [model.selection("SOLID", "Extrusion_2_1")])
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_3 = model.addSketch(Part_2_doc, model.defaultPlane("XOY"))
SketchLine_10 = Sketch_3.addLine(52.9121725731895, 8.799859429739596, 7.781201848998454, 8.799859429739596)
SketchLine_11 = Sketch_3.addLine(7.781201848998454, 8.799859429739596, 7.781201848998454, 36.33111129445148)
SketchLine_12 = Sketch_3.addLine(7.781201848998454, 36.33111129445148, 52.9121725731895, 36.33111129445148)
SketchLine_13 = Sketch_3.addLine(52.9121725731895, 36.33111129445148, 52.9121725731895, 8.799859429739596)
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintHorizontal_5 = Sketch_3.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_6 = Sketch_3.setVertical(SketchLine_11.result())
SketchConstraintHorizontal_6 = Sketch_3.setHorizontal(SketchLine_12.result())
SketchConstraintVertical_7 = Sketch_3.setVertical(SketchLine_13.result())
SketchLine_14 = Sketch_3.addLine(29.87981510015411, 36.33111129445148, 29.87981510015411, 8.799859429739596)
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_14.startPoint(), SketchLine_12.result())
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchLine_10.result())
SketchConstraintVertical_8 = Sketch_3.setVertical(SketchLine_14.result())
SketchCircle_2 = Sketch_3.addCircle(88.70570107858241, 30.96918335901386, 15.95582825781952)
model.do()
Extrusion_3 = model.addExtrusion(Part_2_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 20, 0)
Compound_2 = model.addCompound(Part_2_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Sketch_4 = model.addSketch(Part_2_doc, model.standardPlane("XOY"))
SketchLine_15 = Sketch_4.addLine(91.78842679005474, 52.44197384283513, 18.42664179347724, 52.44197384283513)
SketchLine_16 = Sketch_4.addLine(18.42664179347724, 52.44197384283513, 18.42664179347724, 23.73518840939176)
SketchLine_17 = Sketch_4.addLine(18.42664179347724, 23.73518840939176, 91.78842679005474, 23.73518840939176)
SketchLine_18 = Sketch_4.addLine(91.78842679005474, 23.73518840939176, 91.78842679005474, 52.44197384283513)
SketchConstraintCoincidence_17 = Sketch_4.setCoincident(SketchLine_18.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_18 = Sketch_4.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_19 = Sketch_4.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_20 = Sketch_4.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintHorizontal_7 = Sketch_4.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_9 = Sketch_4.setVertical(SketchLine_16.result())
SketchConstraintHorizontal_8 = Sketch_4.setHorizontal(SketchLine_17.result())
SketchConstraintVertical_10 = Sketch_4.setVertical(SketchLine_18.result())
model.do()
Extrusion_4 = model.addExtrusion(Part_2_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_6r-SketchLine_7f-SketchLine_8f-SketchLine_9f")], model.selection(), 10, 0)
Cut_2 = model.addCut(Part_2_doc, [model.selection("SOLID", "Compound_1_1_1_1")], [model.selection("SOLID", "Extrusion_2_1")], keepSubResults = True)
model.do()

model.end()

from GeomAPI import *

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [16])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [74])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [148])
model.testResultsVolumes(Part_1, [23407.60956])

model.testNbResults(Part_2, 1)
model.testNbSubResults(Part_2, [0])
model.testNbSubShapes(Part_2, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Part_2, GeomAPI_Shape.FACE, [19])
model.testNbSubShapes(Part_2, GeomAPI_Shape.EDGE, [80])
model.testNbSubShapes(Part_2, GeomAPI_Shape.VERTEX, [160])
model.testResultsVolumes(Part_2, [39403.873983562])

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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-88.6588483815807, 35.68247765805569, -60.62456998069012, -7.998839850308682)
SketchLine_2 = Sketch_1.addLine(-60.62456998069012, -7.998839850308682, -33.24225154261096, 37.63835754648991)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(22.82630525917019, 37.63835754648991, 47.60078384600371, -9.954719738742909)
SketchLine_4 = Sketch_1.addLine(47.60078384600371, -9.954719738742909, 74.33114232127146, 36.3344376208671)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_5 = Sketch_2.addLine(-75.23467769579756, 14.76574658950986, -60.62456998069012, -7.998839850308684)
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), False)
SketchLine_6 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_6.result())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_6).endPoint(), SketchLine_5.endPoint())
SketchLine_7 = Sketch_2.addLine(-60.62456998069012, -7.998839850308684, -47.51524103341781, 13.8500417284785)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_7.startPoint())
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_2"), False)
SketchLine_8 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.result())
SketchLine_9 = Sketch_2.addLine(35.74952364371219, 12.81217486039606, 47.60078384600371, -9.954719738742909)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_3"), False)
SketchLine_10 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_9.startPoint(), SketchLine_10.result())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_10).endPoint(), SketchLine_9.endPoint())
SketchLine_11 = Sketch_2.addLine(47.60078384600371, -9.954719738742909, 64.13719888777418, 18.68151118724985)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_11.startPoint())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_4"), False)
SketchLine_12 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.result())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4")])
Wire_3 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_7")])
Wire_4 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_9"), model.selection("EDGE", "Sketch_2/SketchLine_11")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Wire_3_1"), model.selection("WIRE", "Wire_4_1")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())

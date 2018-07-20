## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-64.96569468267583, 22.00171526586621, -41.05831903945112, 55.95711835334479)
SketchLine_1.result().setColor(225, 0, 0)
SketchLine_2 = Sketch_1.addLine(-41.05831903945112, 55.95711835334479, -23.04116638078903, 21.65523156089195)
SketchLine_2.result().setColor(225, 0, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(14.72555746140652, 19.92281303602059, 33.43567753001715, 55.95711835334479)
SketchLine_3.result().setColor(225, 0, 0)
SketchLine_4 = Sketch_1.addLine(33.43567753001715, 55.95711835334479, 50.75986277873073, 22.69468267581476)
SketchLine_4.result().setColor(225, 0, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_5 = Sketch_2.addLine(-67.39108061749573, 66.6981132075472, -42.44425385934821, 35.51457975986279)
SketchLine_5.result().setColor(225, 0, 0)
SketchLine_6 = Sketch_2.addLine(-42.44425385934821, 35.51457975986279, -16.1114922813036, 69.1234991423671)
SketchLine_6.result().setColor(225, 0, 0)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(12.64665523156088, 66.6981132075472, 35.16809605488852, 36.5540308747856)
SketchLine_7.result().setColor(225, 0, 0)
SketchLine_8 = Sketch_2.addLine(35.16809605488852, 36.5540308747856, 52.49228130360206, 66.35162950257292)
SketchLine_8.result().setColor(225, 0, 0)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), model.selection("EDGE", "Sketch_1/Edge-SketchLine_2")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_3"), model.selection("EDGE", "Sketch_1/Edge-SketchLine_4")])
Wire_3 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/Edge-SketchLine_5"), model.selection("EDGE", "Sketch_2/Edge-SketchLine_6")])
Wire_4 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/Edge-SketchLine_7"), model.selection("EDGE", "Sketch_2/Edge-SketchLine_8")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Wire_3_1"), model.selection("WIRE", "Wire_4_1")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [16])

assert(model.checkPythonDump())

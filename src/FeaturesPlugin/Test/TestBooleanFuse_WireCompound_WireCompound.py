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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-74.539241607565, 22.811578250591, -50.12121418439715, 49.79992434988179)
SketchLine_2 = Sketch_1.addLine(-50.12121418439715, 49.79992434988179, -29.55866477541372, 23.45415791962174)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(15.42191205673757, 24.09673758865247, 34.69930212765959, 49.15734468085105)
SketchLine_4 = Sketch_1.addLine(34.69930212765959, 49.15734468085105, 59.75990921985817, 26.02447659574467)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_5 = Sketch_2.addLine(-79.03729929078014, 52.37024302600472, -50.76379385342788, 26.6670562647754)
SketchLine_6 = Sketch_2.addLine(-50.76379385342788, 26.6670562647754, -25.70318676122931, 53.65540236406619)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(13.49417304964538, 51.08508368794326, 35.98446146572105, 27.95221560283687)
SketchLine_8 = Sketch_2.addLine(35.98446146572105, 27.95221560283687, 53.3341125295508, 47.87218534278959)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4")])
Wire_3 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_5"), model.selection("EDGE", "Sketch_2/SketchLine_6")])
Wire_4 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_7"), model.selection("EDGE", "Sketch_2/SketchLine_8")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Wire_3_1"), model.selection("WIRE", "Wire_4_1")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [4])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [16])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [32])

assert(model.checkPythonDump())

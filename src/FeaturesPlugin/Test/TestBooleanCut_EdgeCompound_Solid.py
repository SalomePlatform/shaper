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
SketchLine_1 = Sketch_1.addLine(-28.62948623265935, 53.77363153293705, 40.23707952134417, 53.11775947813702)
SketchLine_2 = Sketch_1.addLine(-26.66187006825925, 26.22700523133564, 40.89295157614425, 26.22700523133564)
model.do()
Compound_1 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_1"), model.selection("EDGE", "Sketch_1/Edge-SketchLine_2")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 10)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_3 = Sketch_2.addLine(14.6580693841429, 60.98822413573743, -10.2650686982584, 60.98822413573743)
SketchLine_4 = Sketch_2.addLine(-10.2650686982584, 60.98822413573743, -10.2650686982584, 18.35654057373522)
SketchLine_5 = Sketch_2.addLine(-10.2650686982584, 18.35654057373522, 14.6580693841429, 18.35654057373522)
SketchLine_6 = Sketch_2.addLine(14.6580693841429, 18.35654057373522, 14.6580693841429, 60.98822413573743)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_6.result())
Extrusion_1.setNestedSketch(Sketch_2)
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())

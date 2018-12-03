## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_1.addCircle(3.367708126821666, -16.14093028927689, 18.51144913400999)
SketchCircle_2 = Sketch_1.addCircle(37.37680688568888, 37.3768068856889, 21.5270932014419)
SketchCircle_3 = Sketch_1.addCircle(-3.554756249974806, 10.65828989222541, 8.503732568675463)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Compound_1_objects = [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_3")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Compound_1_1_1"), model.selection("SOLID", "Compound_1_1_3")], [model.selection("SOLID", "Box_1_1")])
model.do()

model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [3])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Common_1, [15007.623169576811051229014992714])

assert(model.checkPythonDump())

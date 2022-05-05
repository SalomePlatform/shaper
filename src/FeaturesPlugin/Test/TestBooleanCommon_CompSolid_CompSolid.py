# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(47.26932589060148, 52.61651755566245, -44.41221948537944, 52.61651755566245)
SketchLine_2 = Sketch_1.addLine(-44.41221948537944, 52.61651755566245, -44.41221948537944, -26.74931734128092)
SketchLine_3 = Sketch_1.addLine(-44.41221948537944, -26.74931734128092, 47.26932589060148, -26.74931734128092)
SketchLine_4 = Sketch_1.addLine(47.26932589060148, -26.74931734128092, 47.26932589060148, 52.61651755566245)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(7.069530544036858, 52.61651755566245, 10.55455403994589, -26.74931734128092)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_6 = Sketch_2.addLine(95.24043941744291, 32.97847252814945, 25.14447162912627, 32.97847252814945)
SketchLine_7 = Sketch_2.addLine(25.14447162912627, 32.97847252814945, 25.14447162912627, -11.61656767103746)
SketchLine_8 = Sketch_2.addLine(25.14447162912627, -11.61656767103746, 95.24043941744291, -11.61656767103746)
SketchLine_9 = Sketch_2.addLine(95.24043941744291, -11.61656767103746, 95.24043941744291, 32.97847252814945)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_9.result())
SketchLine_10 = Sketch_2.addLine(63.78513134811883, 32.97847252814945, 65.47170548447893, -11.61656767103746)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.startPoint(), SketchLine_6.result())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 10)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 5, 5)
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [1])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [48])

assert(model.checkPythonDump())

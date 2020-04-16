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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-26.93407123287669, 40.82195171232873, -80.80221369863011, 40.82195171232873)
SketchLine_2 = Sketch_1.addLine(-80.80221369863011, 40.82195171232873, -80.80221369863011, -8.837742123287665)
SketchLine_3 = Sketch_1.addLine(-80.80221369863011, -8.837742123287665, -26.93407123287669, -8.837742123287665)
SketchLine_4 = Sketch_1.addLine(-26.93407123287669, -8.837742123287665, -26.93407123287669, 40.82195171232873)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(91.74418013698632, 40.82195171232873, 36.19265821917805, 40.82195171232873)
SketchLine_6 = Sketch_1.addLine(36.19265821917805, 40.82195171232873, 36.19265821917805, -15.57125993150685)
SketchLine_7 = Sketch_1.addLine(36.19265821917805, -15.57125993150685, 91.74418013698632, -15.57125993150685)
SketchLine_8 = Sketch_1.addLine(91.74418013698632, -15.57125993150685, 91.74418013698632, 40.82195171232873)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 100, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_9 = Sketch_2.addLine(-14.30872534246576, 22.30477773972602, -41.24279657534246, 22.30477773972602)
SketchLine_10 = Sketch_2.addLine(-41.24279657534246, 22.30477773972602, -41.24279657534246, 1.262534589041107)
SketchLine_11 = Sketch_2.addLine(-41.24279657534246, 1.262534589041107, -14.30872534246576, 1.262534589041107)
SketchLine_12 = Sketch_2.addLine(-14.30872534246576, 1.262534589041107, -14.30872534246576, 22.30477773972602)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_12.result())
SketchLine_13 = Sketch_2.addLine(123.7283897260274, 22.30477773972602, 74.91038561643839, 22.30477773972602)
SketchLine_14 = Sketch_2.addLine(74.91038561643839, 22.30477773972602, 74.91038561643839, -0.4208448630136968)
SketchLine_15 = Sketch_2.addLine(74.91038561643839, -0.4208448630136968, 123.7283897260274, -0.4208448630136968)
SketchLine_16 = Sketch_2.addLine(123.7283897260274, -0.4208448630136968, 123.7283897260274, 22.30477773972602)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_7 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_8 = Sketch_2.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_8 = Sketch_2.setVertical(SketchLine_16.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 40, -20)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [22])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [96])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [192])

assert(model.checkPythonDump())

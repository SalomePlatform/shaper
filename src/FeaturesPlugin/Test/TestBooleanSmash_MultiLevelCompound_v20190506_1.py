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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 20)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OX"), 40, 2)
LinearCopy_2 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_1_1")], model.selection("EDGE", "PartSet/OY"), 40, 2)
LinearCopy_2.result().subResult(0).subResult(0).setColor(255, 170, 0)
LinearCopy_2.result().subResult(0).subResult(1).setColor(255, 170, 0)
LinearCopy_2.result().subResult(1).subResult(0).setColor(0, 0, 255)
LinearCopy_2.result().subResult(1).subResult(1).setColor(0, 0, 255)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(7, 0, -7, 0)
SketchLine_2 = Sketch_1.addLine(-7, 0, -7, 5)
SketchLine_3 = Sketch_1.addLine(-7, 5, 7, 5)
SketchLine_4 = Sketch_1.addLine(7, 5, 7, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_5 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_3.endPoint(), SketchLine_5.result(), 7, True)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_5).startPoint(), SketchLine_1.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 14)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 5)
SketchLine_6 = Sketch_1.addLine(5, 12, -5, 12)
SketchLine_7 = Sketch_1.addLine(-5, 12, -5, 15)
SketchLine_8 = Sketch_1.addLine(-5, 15, 5, 15)
SketchLine_9 = Sketch_1.addLine(5, 15, 5, 12)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_7.result(), 3)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_8.result(), 10)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_8.endPoint(), SketchLine_5.result(), 5, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchLine_6.startPoint(), SketchLine_3.result(), 7, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 60, 20)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_9r-SketchLine_8r-SketchLine_7r-SketchLine_6r")], model.selection(), 55, 15)
LinearCopy_3 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OY"), 40, 2)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1")], model.selection("EDGE", "PartSet/OZ"), 90, 2)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_3_1"), model.selection("COMPOUND", "AngularCopy_1_1")])
Compound_1.result().subResult(0).subResult(0).setColor(0, 170, 0)
Compound_1.result().subResult(0).subResult(1).setColor(0, 170, 0)
Compound_1.result().subResult(1).subResult(0).setColor(0, 255, 0)
Compound_1.result().subResult(1).subResult(1).setColor(0, 255, 0)
Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "LinearCopy_2_1_1_1")], [model.selection("SOLID", "Compound_1_1_2_1"), model.selection("SOLID", "Compound_1_1_2_2")], keepSubResults = True)

model.testHaveNamingSubshapes(Smash_1, model, Part_1_doc)

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Smash_1, 1)
model.testNbSubResults(Smash_1, [3])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.SOLID, [12])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.FACE, [98])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.EDGE, [408])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.VERTEX, [816])
model.testResultsVolumes(Smash_1, [38810.840569277926988])

assert(model.checkPythonDump())

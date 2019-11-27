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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchCircle_2 = Sketch_1.addCircle(10, 0, 10)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_2.center())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_1.results()[1], SketchCircle_2.results()[1])
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Line(SketchLine_1).startPoint(), SketchCircle_2.center(), 10)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 20, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchCircle_3 = Sketch_2.addCircle(0, -7, 10)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.result(), SketchCircle_3.center())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_3.results()[1], 10)
SketchConstraintDistanceVertical_1 = Sketch_2.setVerticalDistance(SketchCircle_3.center(), SketchAPI_Line(SketchLine_2).startPoint(), 7)
SketchCircle_4 = Sketch_2.addCircle(40, -27, 10)
SketchConstraintEqual_2 = Sketch_2.setEqual(SketchCircle_4.results()[1], SketchCircle_3.results()[1])
SketchConstraintDistanceHorizontal_2 = Sketch_2.setHorizontalDistance(SketchCircle_3.center(), SketchCircle_4.center(), 40)
SketchConstraintDistanceVertical_2 = Sketch_2.setVerticalDistance(SketchCircle_4.center(), SketchCircle_3.center(), 20)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 20, 0)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPSOLID", "Extrusion_1_1")])
Compound_2.result().subResult(0).setColor(255, 170, 0)
Compound_2.result().subResult(0).subResult(0).setColor(255, 170, 0)
Compound_2.result().subResult(0).subResult(1).setColor(255, 170, 0)
Compound_2.result().subResult(1).setColor(0, 85, 255)
Compound_2.result().subResult(1).subResult(0).setColor(0, 85, 255)
Compound_2.result().subResult(1).subResult(1).setColor(0, 85, 255)
Compound_2.result().subResult(1).subResult(2).setColor(0, 85, 255)
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_3 = Sketch_3.addLine(7, 0, -7, 0)
SketchLine_4 = Sketch_3.addLine(-7, 0, -7, 5)
SketchLine_5 = Sketch_3.addLine(-7, 5, 7, 5)
SketchLine_6 = Sketch_3.addLine(7, 5, 7, 0)
SketchConstraintCoincidence_4 = Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_3.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_3.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_3.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_3.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_3.setVertical(SketchLine_6.result())
SketchProjection_3 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_7 = SketchProjection_3.createdFeature()
SketchConstraintDistance_1 = Sketch_3.setDistance(SketchLine_5.endPoint(), SketchLine_7.result(), 7, True)
SketchConstraintMiddle_1 = Sketch_3.setMiddlePoint(SketchAPI_Line(SketchLine_7).startPoint(), SketchLine_3.result())
SketchConstraintLength_1 = Sketch_3.setLength(SketchLine_4.result(), 5)
SketchLine_8 = Sketch_3.addLine(7, 7, -7.000000000000001, 7)
SketchLine_9 = Sketch_3.addLine(-7.000000000000001, 7, -7.000000000000001, 10)
SketchLine_10 = Sketch_3.addLine(-7.000000000000001, 10, 7, 10)
SketchLine_11 = Sketch_3.addLine(7, 10, 7, 7)
SketchConstraintCoincidence_8 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_3.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintHorizontal_3 = Sketch_3.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_3 = Sketch_3.setVertical(SketchLine_9.result())
SketchConstraintHorizontal_4 = Sketch_3.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_4 = Sketch_3.setVertical(SketchLine_11.result())
SketchConstraintCoincidence_12 = Sketch_3.setCoincident(SketchLine_9.startPoint(), SketchLine_4.result())
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_8.startPoint(), SketchLine_6.result())
SketchConstraintLength_2 = Sketch_3.setLength(SketchLine_9.result(), 3)
SketchConstraintDistance_2 = Sketch_3.setDistance(SketchLine_9.startPoint(), SketchLine_5.result(), 2, True)
model.do()
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r")], model.selection(), 50, -10)
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_11r-SketchLine_10r-SketchLine_9r-SketchLine_8r")], model.selection(), 60, 20)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_4_1")], model.selection("EDGE", "PartSet/OZ"), 90, 2)
Compound_3 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_3_1"), model.selection("COMPOUND", "AngularCopy_1_1")])
Compound_3.result().subResult(0).setColor(0, 170, 0)
Compound_3.result().subResult(1).setColor(0, 255, 0)
Compound_3.result().subResult(1).subResult(0).setColor(0, 255, 0)
Compound_3.result().subResult(1).subResult(1).setColor(0, 255, 0)
Smash_1 = model.addSmash(Part_1_doc, [model.selection("SOLID", "Compound_2_1_1_1"), model.selection("SOLID", "Compound_2_1_2_3")], [model.selection("SOLID", "Compound_3_1_2_2")], keepSubResults = True)

model.testHaveNamingSubshapes(Smash_1, model, Part_1_doc)

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Smash_1, 1)
model.testNbSubResults(Smash_1, [2])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.FACE, [83])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.EDGE, [376])
model.testNbSubShapes(Smash_1, GeomAPI_Shape.VERTEX, [752])
model.testResultsVolumes(Smash_1, [27284.1140931756599457])

assert(model.checkPythonDump())

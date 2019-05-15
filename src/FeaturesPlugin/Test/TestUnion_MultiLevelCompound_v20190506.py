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
SketchLine_1 = Sketch_1.addLine(25, -25, -25, -25)
SketchLine_2 = Sketch_1.addLine(-25, -25, -25, 25)
SketchLine_3 = Sketch_1.addLine(-25, 25, 25, 25)
SketchLine_4 = Sketch_1.addLine(25, 25, 25, -25)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 50)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.result(), 25, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.result(), 25, True)
SketchMultiRotation_1_objects = [SketchLine_1.result(), SketchLine_2.result(), SketchLine_3.result(), SketchLine_4.result()]
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchAPI_Point(SketchPoint_1).coordinates(), 45, 2, True)
[SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8] = SketchMultiRotation_1.rotated()
SketchLine_8.setName("SketchLine_14")
SketchLine_8.result().setName("SketchLine_14")
SketchLine_7.setName("SketchLine_11")
SketchLine_7.result().setName("SketchLine_11")
SketchLine_6.setName("SketchLine_8")
SketchLine_6.result().setName("SketchLine_8")
SketchCircle_1 = Sketch_1.addCircle(80, 0, 20)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 20)
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_9 = SketchProjection_2.createdFeature()
SketchLine_9.setName("SketchLine_9")
SketchLine_9.result().setName("SketchLine_9")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_9.result())
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_9).startPoint(), SketchCircle_1.center(), 80, True)
SketchMultiRotation_2 = Sketch_1.addRotation([SketchCircle_1.results()[1]], SketchAPI_Line(SketchLine_9).startPoint(), 360, 8, True)
[SketchCircle_2, SketchCircle_3, SketchCircle_4, SketchCircle_5, SketchCircle_6, SketchCircle_7, SketchCircle_8] = SketchMultiRotation_2.rotated()
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 50, 0)
Compound_1_objects = [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_3"), model.selection("SOLID", "Extrusion_1_4"), model.selection("SOLID", "Extrusion_1_5"), model.selection("SOLID", "Extrusion_1_6"), model.selection("SOLID", "Extrusion_1_7"), model.selection("SOLID", "Extrusion_1_8"), model.selection("SOLID", "Extrusion_1_9")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
Union_1_objects = [model.selection("SOLID", "Compound_1_1_1_1"), model.selection("SOLID", "Compound_1_1_1_2"), model.selection("SOLID", "Compound_1_1_1_3")]
Union_1 = model.addUnion(Part_1_doc, Union_1_objects, 20190506)

model.testHaveNamingSubshapes(Union_1, model, Part_1_doc)

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Union_1, 1)
model.testNbSubResults(Union_1, [9])
model.testNbSubShapes(Union_1, GeomAPI_Shape.SOLID, [15])
model.testNbSubShapes(Union_1, GeomAPI_Shape.FACE, [70])
model.testNbSubShapes(Union_1, GeomAPI_Shape.EDGE, [224])
model.testNbSubShapes(Union_1, GeomAPI_Shape.VERTEX, [448])
model.testResultsVolumes(Union_1, [649101.433981])

assert(model.checkPythonDump())

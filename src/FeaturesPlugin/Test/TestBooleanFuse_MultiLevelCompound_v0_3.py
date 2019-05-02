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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OY"), 15, 3)
LinearCopy_2 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_1_1")], model.selection("EDGE", "PartSet/OX"), -20, 2)
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(10, -5, 0, -5)
SketchLine_2 = Sketch_1.addLine(0, -5, 0, 0)
SketchLine_3 = Sketch_1.addLine(0, 0, 10, 0)
SketchLine_4 = Sketch_1.addLine(10, 0, 10, -5)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(10, -5, 20, -5)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(20, -5, 20, 0)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(20, 0, 10, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_7.endPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_5.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_8 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchAPI_Line(SketchLine_8).startPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_10")
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 5)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 10)
Extrusion_1.setNestedSketch(Sketch_1)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_2/From_Face"))
SketchLine_9 = Sketch_2.addLine(22, 2.5, 6, 2.5)
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_9.result(), 16)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4][Extrusion_1_1_1/From_Face]"), False)
SketchLine_10 = SketchProjection_2.createdFeature()
SketchPoint_1 = Sketch_2.addPoint(10, 2.5)
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchPoint_1.coordinates(), SketchLine_9.result())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_11")
SketchConstraintMiddle_1 = Sketch_2.setMiddlePoint(SketchPoint_1.coordinates(), SketchLine_10.result())
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_9.startPoint(), SketchLine_10.result(), 12, True)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_9")])
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OZ"), 13, 3)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("FACE", "Extrusion_2_1")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Compound_1_1_1_1")], [model.selection("SOLID", "LinearCopy_2_1_1_1"), model.selection("FACE", "Compound_1_1_2")], True)

model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [16])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [78])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [156])
model.testResultsVolumes(Fuse_1, [1589.048622670478835])

assert(model.checkPythonDump())

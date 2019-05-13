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
SketchCircle_1 = Sketch_1.addCircle(10, 25, 5)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), 10)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), 25)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 5)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OZ"), 30, 2)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "AngularCopy_1_1")], model.selection("EDGE", "PartSet/OY"), 20, 2)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "LinearCopy_1_1_1_2/MF:Translated&Extrusion_1_1/To_Face"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "[LinearCopy_1_1_1_2/MF:Translated&Sketch_1/SketchCircle_1_2][LinearCopy_1_1_1_2/MF:Translated&Extrusion_1_1/To_Face]__cc"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(-3.839745962155611, 26.65063509461097, 3)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchPoint_2.result(), SketchCircle_2.center())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_2.results()[1], 3)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection(), 5, 5)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "LinearCopy_1_1_1_2")], [model.selection("SOLID", "Extrusion_2_1")], 20190506)

model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [14])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Cut_1, [3000.22098417825282])

assert(model.checkPythonDump())

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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 50)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OX"), 50, 2)
LinearCopy_2 = model.addMultiTranslation(Part_1_doc, [model.selection("COMPOUND", "LinearCopy_1_1")], model.selection("EDGE", "PartSet/OY"), 50, 3)
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "([LinearCopy_2_1_2_1/MF:Translated&Cylinder_1_1/Face_1][LinearCopy_2_1_2_1/MF:Translated&Cylinder_1_1/Face_2])([LinearCopy_2_1_2_1/MF:Translated&Cylinder_1_1/Face_1][LinearCopy_2_1_2_1/MF:Translated&Cylinder_1_1/Face_3])"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(50, 25, 5)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 5)
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchCircle_1.center(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 100, 0)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "LinearCopy_2_1_2_2")], [model.selection("SOLID", "Extrusion_1_1")], keepSubResults = True)
Recover_1 = model.addRecover(Part_1_doc, Cut_1, [LinearCopy_2.result()], True)
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [12])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [37])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [82])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [164])
model.testResultsVolumes(Part_1, [186975.51922585917])

assert(model.checkPythonDump())

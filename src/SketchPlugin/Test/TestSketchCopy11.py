# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchIntersectionPoint_1 = Sketch_1.addIntersectionPoint(model.selection("EDGE", "[Cylinder_1_1/Face_1][Cylinder_1_1/Face_2]"), True)
[SketchPoint_1, SketchPoint_2] = SketchIntersectionPoint_1.intersectionPoints()
SketchCircle_1 = Sketch_1.addCircle(5, 10, 3)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 3)
SketchMultiTranslation_1 = Sketch_1.addTranslation([SketchCircle_1.results()[1]], SketchAPI_Point(SketchPoint_2).coordinates(), SketchAPI_Point(SketchPoint_1).coordinates(), 3)
[SketchCircle_2, SketchCircle_3] = SketchMultiTranslation_1.translated()
SketchMultiRotation_1 = Sketch_1.addRotation([SketchCircle_1.results()[1]], SketchAPI_Point(SketchPoint_2).coordinates(), 360, 5, True)
[SketchCircle_4, SketchCircle_5, SketchCircle_6, SketchCircle_7] = SketchMultiRotation_1.rotated()
SketchLine_1 = Sketch_1.addLine(2.699367028097859, 1.399671792347037, 12.16381438587305, 5.532036131657337)
SketchLine_1.setAuxiliary(True)
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchLine_1.result())
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_1.result(), [SketchCircle_1.results()[1]])
[SketchCircle_8] = SketchConstraintMirror_1.mirrored()
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)

Sketch_1_Copy = model.copySketch(Part_1_doc, Sketch_1)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", Sketch_1_Copy.name())], model.selection(), 0, 10)

model.end()

error = model.compareSketches(Sketch_1, Sketch_1_Copy)
assert(error == ""), error

VOL = 282.743338823

model.testNbResults(Extrusion_1, 8)
model.testNbSubResults(Extrusion_1, [0, 0, 0, 0, 0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1, 1, 1, 1, 1, 1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [3, 3, 3, 3, 3, 3, 3, 3])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [6, 6, 6, 6, 6, 6, 6, 6])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [12, 12, 12, 12, 12, 12, 12, 12])
model.testResultsVolumes(Extrusion_1, [VOL, VOL, VOL, VOL, VOL, VOL, VOL, VOL])

model.testNbResults(Extrusion_2, 8)
model.testNbSubResults(Extrusion_2, [0, 0, 0, 0, 0, 0, 0, 0])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [1, 1, 1, 1, 1, 1, 1, 1])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [3, 3, 3, 3, 3, 3, 3, 3])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [6, 6, 6, 6, 6, 6, 6, 6])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [12, 12, 12, 12, 12, 12, 12, 12])
model.testResultsVolumes(Extrusion_2, [VOL, VOL, VOL, VOL, VOL, VOL, VOL, VOL])

assert(Part_1_doc.size("Features") == 5)

assert(model.checkPythonDump())

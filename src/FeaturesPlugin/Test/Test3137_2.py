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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 20, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(15.15541180270434, 24.41800127226657, 10.26187655283108)
Extrusion_1.setNestedSketch(Sketch_1)
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Extrusion_2 = model.addExtrusion(Part_2_doc, [], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_2_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_2.addLine(-19.91316598300577, 19.16479847747027, -37.02832010642763, 19.16479847747027)
SketchLine_2 = Sketch_2.addLine(-37.02832010642763, 19.16479847747027, -37.02832010642763, 36.27995260089213)
SketchLine_3 = Sketch_2.addLine(-37.02832010642763, 36.27995260089213, -19.91316598300577, 36.27995260089213)
SketchLine_4 = Sketch_2.addLine(-19.91316598300577, 36.27995260089213, -19.91316598300577, 19.16479847747027)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_4.result(), SketchLine_3.result())
Extrusion_2.setNestedSketch(Sketch_2)
model.do()

AngularCopy_1 = model.addMultiRotation(partSet, [model.selection("COMPOUND", "Part_1/"), model.selection("COMPOUND", "Part_2/")], model.selection("EDGE", "OZ"), 90, 2)
model.end()

from GeomAPI import *

model.testNbResults(AngularCopy_1, 4)
model.testNbSubResults(AngularCopy_1, [0, 0, 0, 0])
model.testNbSubShapes(AngularCopy_1, GeomAPI_Shape.SOLID, [1, 1, 1, 1])
model.testNbSubShapes(AngularCopy_1, GeomAPI_Shape.FACE, [3, 3, 6, 6])
model.testNbSubShapes(AngularCopy_1, GeomAPI_Shape.EDGE, [6, 6, 24, 24])
model.testNbSubShapes(AngularCopy_1, GeomAPI_Shape.VERTEX, [12, 12, 48, 48])
model.testResultsVolumes(AngularCopy_1, [6616.5780553, 6616.5780553, 2929.285, 2929.285])

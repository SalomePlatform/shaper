# Copyright (C) 2020-2023  CEA, EDF
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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(4, 5.137343601256935, 3)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 3)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 4, True)
ExtrusionCut_1.setNestedSketch(Sketch_1)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Back"), model.selection("FACE", "Box_1_1/Front"))
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_1_1"), model.selection("FACE", "Plane_1")], keepSubResults = True)
Defeaturing_1_objects = [model.selection("FACE", "Partition_1_1_2/Modified_Face&Sketch_1/SketchCircle_1_2&weak_name_2"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Sketch_1/SketchCircle_1_2&weak_name_1"), model.selection("FACE", "Partition_1_1_1/Modified_Face&Sketch_1/SketchCircle_1_2")]
Defeaturing_1 = model.addDefeaturing(Part_1_doc, Defeaturing_1_objects)
model.testHaveNamingSubshapes(Defeaturing_1, model, Part_1_doc)
model.end()

from GeomAPI import *

model.testNbResults(Defeaturing_1, 1)
model.testNbSubResults(Defeaturing_1, [2])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.FACE, [12])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Defeaturing_1, [1000])

assert(model.checkPythonDump())

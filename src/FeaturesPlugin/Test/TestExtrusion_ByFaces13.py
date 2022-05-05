# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
ParamR = model.addParameter(Part_1_doc, "R", "50")
ParamFrom = model.addParameter(Part_1_doc, "offsetFrom", "0")
ParamTo = model.addParameter(Part_1_doc, "offsetTo", "0")
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R", 100, 180)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(20, 40, -30, 40)
SketchLine_2 = Sketch_1.addLine(-30, 40, -30, 20)
SketchLine_3 = Sketch_1.addLine(-30, 20, 20, 20)
SketchLine_4 = Sketch_1.addLine(20, 20, 20, 40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 50)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 20)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.endPoint(), 20)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.endPoint(), 20)
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Extrusion_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3"), model.selection("EDGE", "Edge_1_4")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection("EDGE", "PartSet/OY"), model.selection("FACE", "Cylinder_1_1/Face_1"), "offsetTo", model.selection(), "offsetFrom")

from GeomAPI import GeomAPI_Shape

model.testNbResults(Extrusion_1, 4)
model.testNbSubResults(Extrusion_1, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 4, 4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 8, 8])
model.testResultsAreas(Extrusion_1, [2377.030013, 800, 2377.030013, 916.515139])

# change radius of cylinder
ParamR.setValue(10)
model.do()
assert(Extrusion_1.feature().error() != "")

ParamR.setValue(30)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure and check the intersected boundaries are processed well
ParamR.setValue(40);
model.do()
model.testNbResults(Extrusion_1, 4)
model.testNbSubResults(Extrusion_1, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 4, 4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 8, 8])
model.testResultsAreas(Extrusion_1, [1840.601542, 529.150262, 1840.601542, 692.820323])

# change offsetting "To" face
ParamTo.setValue(-20)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure
ParamTo.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 4)
model.testNbSubResults(Extrusion_1, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 4, 4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 8, 8])
model.testResultsAreas(Extrusion_1, [2899.292521, 1039.2304845, 2899.292521, 1131.3708499])

# offset "From" face
ParamFrom.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 4)
model.testNbSubResults(Extrusion_1, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 4, 4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 8, 8])
model.testResultsAreas(Extrusion_1, [3899.292521, 1439.2304845, 3899.292521, 1531.3708499])

ParamFrom.setValue(-20)
model.do()
model.testNbResults(Extrusion_1, 4)
model.testNbSubResults(Extrusion_1, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 4, 4])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 8, 8])
model.testResultsAreas(Extrusion_1, [1899.292521, 639.2304845, 1899.292521, 731.3708499])

model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())

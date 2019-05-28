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
ParamR = model.addParameter(Part_1_doc, "R", "50")
ParamFrom = model.addParameter(Part_1_doc, "offsetFrom", "0")
ParamTo = model.addParameter(Part_1_doc, "offsetTo", "0")
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R", "R/5", "R")
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Cone_1_1"), model.selection("FACE", "PartSet/XOZ")])
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), 20, False)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), model.selection("EDGE", "PartSet/OZ"), 330)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(10, 8.819660112501044, 10, 30)
SketchLine_2 = Sketch_1.addLine(10, 30, -11.18033988749895, 30)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchArc_1 = Sketch_1.addArc(0, 20, -11.18033988749895, 30, 10, 8.819660112501044, False)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_2.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.center(), SketchLine_1.result(), 10, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 15)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_3 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_3.result())
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Line(SketchLine_3).startPoint(), SketchArc_1.center(), 20)
model.do()
Edge_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchArc_1_2")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Extrusion_1_objects = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3")]
Extrusion_1 = model.addExtrusion(Part_1_doc, Extrusion_1_objects, model.selection("EDGE", "PartSet/OY"), model.selection("FACE", "Partition_1_1_2/Modified_Face&Cone_1_1/Face_1"), "offsetTo", model.selection("FACE", "Plane_2"), "offsetFrom")

from GeomAPI import GeomAPI_Shape

model.testNbResults(Extrusion_1, 3)
model.testNbSubResults(Extrusion_1, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 5])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 10])
model.testResultsVolumes(Extrusion_1, [1309.42058662, 1016.82520496, 2589.5946928])

# change radius of sphere and check failure
ParamR.setValue(10)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure, check the intersected boundaries are processed well
ParamR.setValue(40)
model.do()
model.testNbResults(Extrusion_1, 3)
model.testNbSubResults(Extrusion_1, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 5])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 10])
model.testResultsVolumes(Extrusion_1, [1081.93830116, 793.80218227, 2095.8021933])

# change offsetting "To" face and check failure
ParamTo.setValue(-20)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure
ParamTo.setValue(8)
model.do()
model.testNbResults(Extrusion_1, 3)
model.testNbSubResults(Extrusion_1, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [4, 4, 5])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [8, 8, 10])
model.testResultsVolumes(Extrusion_1, [1314.8586588, 1022.169915122, 2601.32913399])

# offset "From" face
ParamFrom.setValue(-20)
model.do()
model.testNbResults(Extrusion_1, 3)
model.testNbSubResults(Extrusion_1, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1])
model.testResultsVolumes(Extrusion_1, [825.71966141, 533.030917699, 1551.6191757579])

ParamFrom.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 3)
model.testNbSubResults(Extrusion_1, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0, 0, 0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1, 1, 1])
model.testResultsVolumes(Extrusion_1, [1803.997656259, 1511.308912545, 3651.0390970659])

model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())

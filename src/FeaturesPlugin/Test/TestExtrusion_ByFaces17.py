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
ParamRSph = model.addParameter(Part_1_doc, "rSphere", "50")
ParamRCyl = model.addParameter(Part_1_doc, "rCyl", "30")
ParamRCir = model.addParameter(Part_1_doc, "rCircle", "10")
ParamFrom = model.addParameter(Part_1_doc, "offsetFrom", "0")
ParamTo = model.addParameter(Part_1_doc, "offsetTo", "0")
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "rSphere")
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "rCyl", 100)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchCircle_1 = Sketch_1.addCircle(15, 20, 10)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "rCircle")
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), 15)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), 20)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OY"), model.selection("FACE", "Sphere_1_1/Face_1"), "offsetTo", model.selection("FACE", "Cylinder_1_1/Face_1"), "offsetFrom")

from GeomAPI import GeomAPI_Shape

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsAreas(Extrusion_1, [2178.8497332])

# change radius of sketch circle
ParamRCir.setValue(17)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [10])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [20])
model.testResultsAreas(Extrusion_1, [3960.00723])

# change radius of sphere
ParamRSph.setValue(10)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [5])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [10])
model.testResultsAreas(Extrusion_1, [3658.7714])

# change radius of sketch circle and check failure
ParamRCir.setValue(10)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure
ParamRSph.setValue(45);
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsAreas(Extrusion_1, [1398.72992867])

# change radius of cylinder
ParamRCyl.setValue(10)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [12])
model.testResultsAreas(Extrusion_1, [4194.25709])

ParamRCyl.setValue(50)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsAreas(Extrusion_1, [1433.0518])

# change offsetting "To" face
ParamTo.setValue(-20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [10])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [20])
model.testResultsAreas(Extrusion_1, [5078.347278])

# revert failure
ParamTo.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsAreas(Extrusion_1, [1504.36096473])

# offset "From" face
ParamFrom.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsAreas(Extrusion_1, [4336.142699])

ParamFrom.setValue(-20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsAreas(Extrusion_1, [1118.8887326])

model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())

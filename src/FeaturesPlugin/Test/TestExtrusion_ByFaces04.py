# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
from GeomAPI import GeomAPI_Shape

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamR = model.addParameter(Part_1_doc, "R", "50")
ParamSize = model.addParameter(Part_1_doc, "Size", "50")
ParamFrom = model.addParameter(Part_1_doc, "offsetFrom", "0")
ParamTo = model.addParameter(Part_1_doc, "offsetTo", "0")
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "R")
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Sphere_1_1"), model.selection("FACE", "PartSet/XOZ")])
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), 30, False)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_1 = Sketch_1.addLine(50, -50, -50, -50)
SketchLine_2 = Sketch_1.addLine(-50, -50, -50, 50)
SketchLine_3 = Sketch_1.addLine(-50, 50, 50, 50)
SketchLine_4 = Sketch_1.addLine(50, 50, 50, -50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_4.result(), "Size", True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_2.result(), "Size", True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_1.result(), "Size", True)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_3.result(), "Size", True)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")])
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_5 = Sketch_2.addLine(20, -40.3112887414928, 20, 20)
SketchLine_6 = Sketch_2.addLine(20, 20, -40.31128874149281, 20.00000000000001)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchArc_1 = Sketch_2.addArc(0, 0, -40.31128874149281, 20.00000000000001, 20, -40.3112887414928, False)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchArc_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintEqual_1 = Sketch_2.setEqual(SketchLine_5.result(), SketchLine_6.result())
SketchConstraintDistance_5 = Sketch_2.setDistance(SketchArc_1.center(), SketchLine_5.result(), 20, True)
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchArc_1.center())
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchArc_1.results()[1], 45)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_5r-SketchLine_6f-SketchArc_1_2f")], model.selection(), model.selection("FACE", "Partition_1_1_1/Modified_Face&Sphere_1_1/Face_1"), "offsetTo", model.selection("FACE", "Face_1_1"), "offsetFrom")

# extrusion have to fail because of several results with shared topology
assert(Extrusion_1.feature().error() != "")

# change radius of sphere and check failure
ParamR.setValue(10)
model.do()
assert(Extrusion_1.feature().error() != "")

ParamR.setValue(45)
model.do()
assert(Extrusion_1.feature().error() != "")

# change offsetting "To" face and check failure
ParamTo.setValue(-20)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure
ParamTo.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [5])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [36])
model.testResultsVolumes(Extrusion_1, [105008.93])

# offset "From" face
ParamFrom.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [5])
model.testResultsVolumes(Extrusion_1, [179594.5])

# check failure
ParamFrom.setValue(-20)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure
ParamTo.setValue(0)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [5])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [21])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [42])
model.testResultsVolumes(Extrusion_1, [62569.416])

model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())

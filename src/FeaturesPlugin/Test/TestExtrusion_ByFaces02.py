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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R", 100, 180)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OY"), 50)
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
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_1"), "offsetTo", model.selection(), "offsetFrom")

from GeomAPI import GeomAPI_Shape

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Extrusion_1, [97540.6])

# change radius of cylinder and check failure
ParamR.setValue(10)
model.do()
assert(Extrusion_1.feature().error() != "")

# revert failure, check the intersected boundaries are processed well
ParamR.setValue(30)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testResultsVolumes(Extrusion_1, [75176.8518])

ParamR.setValue(40);
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testResultsVolumes(Extrusion_1, [86812.03])

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
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testResultsVolumes(Extrusion_1, [107985.85])

# offset "From" face
ParamFrom.setValue(-20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testResultsVolumes(Extrusion_1, [87985.85])

ParamFrom.setValue(20)
model.do()
model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Extrusion_1, [127985.85])

model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())

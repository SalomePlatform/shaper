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

# Check the specification case of move to the end and split (#3059)

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 12, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(33.32502963835739, 19.24021483244179, 5)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 5)
SketchLine_1 = Sketch_1.addLine(0, 0, 33.32502963835739, 19.24021483244179)
SketchLine_1.setAuxiliary(True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.endPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_2.result(), SketchLine_1.result(), 30)
Extrusion_1.setNestedSketch(Sketch_1)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Extrusion_1_1/To_Face")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OZ"), 12)
model.do()
Part_1_doc.moveFeature(Group_1.feature(), AngularCopy_1.feature(), True)
model.end()

# must be created 12 groups of faces, 12 results
assert(Part_1_doc.size("Groups") == 12)

for i in range(12):
  resShape = modelAPI_Result(Part_1_doc.object("Groups", i)).shape()
  assert(not resShape.isNull())
  # the group result is a compund, check that this is a compound of one face
  aShapeExplorer = GeomAPI_ShapeExplorer(resShape, GeomAPI_Shape.FACE)
  assert(aShapeExplorer.more())
  assert(aShapeExplorer.current().isFace())
  aShapeExplorer.next()
  assert(not aShapeExplorer.more())

assert(model.checkPythonDump())

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

# Check the case when 2 faces in one solid selected in group with thranslation x5, moves to 5 translated groups
# This is an additional request in the issue #3059

from salome.shaper import model
from ModelAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(45.88138045990679, 57.99590690678009, 10.80094646219014)
SketchLine_1 = Sketch_1.addLine(40.23195449216883, 67.2015807394968, 46.4026355638949, 47.20754569686197)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
model.do()
Sketch_1.changeFacesOrder([[SketchCircle_1.results()[1], SketchCircle_1.results()[1], SketchLine_1.result()],
                           [SketchCircle_1.results()[1], SketchLine_1.result()]
                          ])
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_1_2r-SketchLine_1f")], model.selection(), 10, 0)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Extrusion_1_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1_2/To_Face")])
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OZ"), 5, keepSubResults = True)
model.do()
Part_1_doc.moveFeature(Group_1.feature(), AngularCopy_1.feature(), True)
model.end()

# must be created 5 groups of faces, 2 faces in each
assert(Part_1_doc.size("Groups") == 5)

for i in range(5):
  resShape = modelAPI_Result(Part_1_doc.object("Groups", i)).shape()
  assert(not resShape.isNull())
  # the group result is a compund, check that this is a compound of two faces
  aShapeExplorer = GeomAPI_ShapeExplorer(resShape, GeomAPI_Shape.FACE)
  assert(aShapeExplorer.more())
  assert(aShapeExplorer.current().isFace())
  aShapeExplorer.next()
  assert(aShapeExplorer.more())
  assert(aShapeExplorer.current().isFace())
  aShapeExplorer.next()
  assert(not aShapeExplorer.more())

assert(model.checkPythonDump())

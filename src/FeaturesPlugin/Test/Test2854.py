## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

# Test for the order of extrusion results: even compsolids or solids, they depend on the order
# of created sketch entities

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20.52339901477833, 44.52955665024631, -23.88177339901478, 44.52955665024631)
SketchLine_2 = Sketch_1.addLine(-23.88177339901478, 44.52955665024631, -23.88177339901478, 15.29926108374386)
SketchLine_3 = Sketch_1.addLine(-23.88177339901478, 15.29926108374386, 20.52339901477833, 15.29926108374386)
SketchLine_4 = Sketch_1.addLine(20.52339901477833, 15.29926108374386, 20.52339901477833, 44.52955665024631)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(29.23029556650247, 27.36453201970445, -5.970443349753695, 27.36453201970445)
SketchLine_6 = Sketch_1.addLine(-5.970443349753695, 27.36453201970445, -5.970443349753695, -13.80665024630542)
SketchLine_7 = Sketch_1.addLine(-5.970443349753695, -13.80665024630542, 29.23029556650247, -13.80665024630542)
SketchLine_8 = Sketch_1.addLine(29.23029556650247, -13.80665024630542, 29.23029556650247, 27.36453201970445)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
Group_1_objects = [model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_7"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_8"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Extrusion_1_1_3/From_Face"), model.selection("FACE", "Extrusion_1_1_3/To_Face"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1_3/Generated_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_2"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Extrusion_1_1_1/To_Face"), model.selection("FACE", "Extrusion_1_1_1/From_Face"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1"), model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_4"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_6"), model.selection("FACE", "Extrusion_1_1_2/From_Face"), model.selection("FACE", "Extrusion_1_1_2/To_Face"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_5"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_4")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.do()
# add a circle to the sketch
SketchCircle_1 = Sketch_1.addCircle(-26.84625185927467, -13.07763963304469, 9.031583682868897)
model.end()

# check group is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))

selectionList = Group_1.feature().selectionList("group_list")
assert(selectionList.size() == 22)
for i in range(22):
  assert(Group_1.groupList().value(i).value().shapeType() == GeomAPI_Shape.FACE)

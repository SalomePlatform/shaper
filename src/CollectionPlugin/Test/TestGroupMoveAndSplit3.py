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

# Check the movement to the end and split: whole results, check names of splitted results and groups

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-8.333743842364534, 20.52339901477833, -20.15024630541872, 20.52339901477833)
SketchLine_2 = Sketch_1.addLine(-20.15024630541872, 20.52339901477833, -20.15024630541872, 3.980295566502462)
SketchLine_3 = Sketch_1.addLine(-20.15024630541872, 3.980295566502462, -8.333743842364534, 3.980295566502462)
SketchLine_4 = Sketch_1.addLine(-8.333743842364534, 3.980295566502462, -8.333743842364534, 20.52339901477833)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(5.721674876847291, 12.81157635467982, 6.421166795138789)
Extrusion_1.setNestedSketch(Sketch_1)
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Group_1.setName("GroupResult")
Group_1.result().setName("GroupResult")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchLine_5 = Sketch_2.addLine(3.924377723198604, 15.23693857548147, -14.36967929032953, 15.23693857548147)
SketchLine_6 = Sketch_2.addLine(-14.36967929032953, 15.23693857548147, -14.36967929032953, 11.61585476914922)
SketchLine_7 = Sketch_2.addLine(-14.36967929032953, 11.61585476914922, 3.924377723198604, 11.61585476914922)
SketchLine_8 = Sketch_2.addLine(3.924377723198604, 11.61585476914922, 3.924377723198604, 15.23693857548147)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchLine_5r-SketchLine_6f-SketchLine_7f-SketchLine_8f_wire")], model.selection(), 2, 5)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1")], keepSubResults = True)
model.do()
Part_1_doc.moveFeature(Group_1.feature(), Fuse_1.feature(), True)
model.end()

assert(Part_1_doc.size("Groups") == 2) # 2 results because initially 2 results were selected

# check names of results
from ModelAPI import *
res1 = modelAPI_Result(Part_1_doc.object("Groups", 0))
assert(res1.data().name() == "GroupResult_1")
res2 = modelAPI_Result(Part_1_doc.object("Groups", 1))
assert(res2.data().name() == "GroupResult_2")

assert(model.checkPythonDump())

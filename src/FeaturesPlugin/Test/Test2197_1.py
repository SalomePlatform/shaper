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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(-150, 79, -50, 79)
SketchCircle_2 = Sketch_1.addCircle(-155, 278, -10, 278)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchCircle_2_2f")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Revolution_1_1/Base_Edge_1"), model.selection("EDGE", "Revolution_1_1/Base_Edge_2"), model.selection("EDGE", "Revolution_1_1/Base_Edge_3")])
Group_2 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Revolution_1_1/Lateral_Edge_1"), model.selection("EDGE", "Revolution_1_1/Lateral_Edge_2"), model.selection("EDGE", "Revolution_1_1/Lateral_Edge_3")])
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
for aGroupIter in [Group_1, Group_2]:
  assert(aFactory.validate(aGroupIter.feature()))
  assert(aGroupIter.groupList().size() == 3)
  for a in range(3):
    assert(aGroupIter.groupList().value(a).value().shapeTypeStr() == "EDGE")

assert(model.checkPythonDump())

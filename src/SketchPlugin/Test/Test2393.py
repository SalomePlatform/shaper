# Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

"""
    Test2393.py
    Test case for issue #2393 "Constraint conflict when doing an angular copy of a projected edge"
"""


from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Top"), 10, False)

Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"), True)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_1.result()], SketchAPI_Point(SketchPoint_1).coordinates(), 90, 3)
[SketchLine_1, SketchLine_2, SketchLine_3] = SketchMultiRotation_1.rotated()
model.do()

model.checkSketch(Sketch_1, 0)

SketchLine_2 = SketchAPI_Line(SketchLine_2)
Sketch_1.move(SketchLine_2.startPoint(), SketchLine_2.startPoint().x() + 1, SketchLine_2.startPoint().y() + 1)
model.do()

model.checkSketch(Sketch_1, 0)

model.end()

assert(model.checkPythonDump())

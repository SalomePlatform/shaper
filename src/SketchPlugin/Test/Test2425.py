## Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

"""
    Test2425.py
    Test case for issue #2425 "undo edit parameter - destroy document"
"""


from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Param = model.addParameter(partSet, "n", "5")
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(10, 50, 20, 50)
SketchLine_2 = Sketch_1.addLine(20, 50, 20, 60)
SketchLine_3 = Sketch_1.addLine(20, 60, 10, 60)
SketchLine_4 = Sketch_1.addLine(10, 60, 10, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(10, 10, 90, 10)
SketchMultiTranslation_1_objects = [SketchLine_2.result(), SketchLine_1.result(), SketchLine_3.result(), SketchLine_4.result()]
SketchMultiTranslation_1 = Sketch_1.addTranslation(SketchMultiTranslation_1_objects, SketchLine_5.startPoint(), SketchLine_5.endPoint(), "n", True)
[SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12, SketchLine_13, SketchLine_14, SketchLine_15, SketchLine_16, SketchLine_17, SketchLine_18, SketchLine_19, SketchLine_20, SketchLine_21] = SketchMultiTranslation_1.translated()
model.do()

# change parameter, check original lines are not changed
Param.setValue(3)
assert(SketchLine_1.startPoint().x() == 10 and SketchLine_1.startPoint().y() == 50)
assert(SketchLine_1.endPoint().x() == 20 and SketchLine_1.endPoint().y() == 50)
assert(SketchLine_2.startPoint().x() == 20 and SketchLine_2.startPoint().y() == 50)
assert(SketchLine_2.endPoint().x() == 20 and SketchLine_2.endPoint().y() == 60)
assert(SketchLine_3.startPoint().x() == 20 and SketchLine_3.startPoint().y() == 60)
assert(SketchLine_3.endPoint().x() == 10 and SketchLine_3.endPoint().y() == 60)
assert(SketchLine_4.startPoint().x() == 10 and SketchLine_4.startPoint().y() == 60)
assert(SketchLine_4.endPoint().x() == 10 and SketchLine_4.endPoint().y() == 50)

model.end()

# undo parameter changing, check original lines are not changed
model.undo()
assert(SketchLine_1.startPoint().x() == 10 and SketchLine_1.startPoint().y() == 50)
assert(SketchLine_1.endPoint().x() == 20 and SketchLine_1.endPoint().y() == 50)
assert(SketchLine_2.startPoint().x() == 20 and SketchLine_2.startPoint().y() == 50)
assert(SketchLine_2.endPoint().x() == 20 and SketchLine_2.endPoint().y() == 60)
assert(SketchLine_3.startPoint().x() == 20 and SketchLine_3.startPoint().y() == 60)
assert(SketchLine_3.endPoint().x() == 10 and SketchLine_3.endPoint().y() == 60)
assert(SketchLine_4.startPoint().x() == 10 and SketchLine_4.startPoint().y() == 60)
assert(SketchLine_4.endPoint().x() == 10 and SketchLine_4.endPoint().y() == 50)

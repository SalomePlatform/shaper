# Copyright (C) 2014-2024  CEA, EDF
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
    Test2239.py
    Test case for issue #2239 "Sketch not updated after having changed the parameter of multi-translation to 1"
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Parameter_N = model.addParameter(Part_1_doc, "N", "5")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(20, 0, 0, 0)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 20)
SketchLine_3 = Sketch_1.addLine(0, 20, 20, 20)
SketchLine_4 = Sketch_1.addLine(20, 20, 20, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_3.result(), SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 20)
SketchLine_5 = Sketch_1.addLine(20, 0, 30, 0)
SketchLine_5.setAuxiliary(True)
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.startPoint())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_5.result(), 10)
SketchMultiTranslation_1_objects = [SketchLine_1.result(), SketchLine_2.result(), SketchLine_3.result(), SketchLine_4.result()]
SketchMultiTranslation_1 = Sketch_1.addTranslation(SketchMultiTranslation_1_objects, SketchLine_1.endPoint(), SketchLine_5.endPoint(), "N")
model.do()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Sketch_1, 1)
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.EDGE, [20])
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.VERTEX, [40])

# check MultiTranslation error
Parameter_N.setValue(1)
model.do()
assert(SketchMultiTranslation_1.feature().error() != "")


# check MultiTranslation error disappears
Parameter_N.setValue(2)
model.do()
assert(SketchMultiTranslation_1.feature().error() == "")

model.testNbResults(Sketch_1, 1)
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.VERTEX, [16])

# check MultiTranslation is still correct
Parameter_N.setValue(5)
model.do()
assert(SketchMultiTranslation_1.feature().error() == "")

model.testNbResults(Sketch_1, 1)
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.EDGE, [20])
model.testNbSubShapes(Sketch_1, GeomAPI_Shape.VERTEX, [40])

model.end()


assert(model.checkPythonDump())

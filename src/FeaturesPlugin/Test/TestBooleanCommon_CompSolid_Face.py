# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(21.16527496861552, 26.63979884904572, -22.80735826270614, 26.63979884904572)
SketchLine_2 = Sketch_1.addLine(-22.80735826270614, 26.63979884904572, -22.80735826270614, -13.87637783487681)
SketchLine_3 = Sketch_1.addLine(-22.80735826270614, -13.87637783487681, 21.16527496861552, -13.87637783487681)
SketchLine_4 = Sketch_1.addLine(21.16527496861552, -13.87637783487681, 21.16527496861552, 26.63979884904572)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-2.257333687264361, 26.63979884904572, -2.257333687264361, -13.87637783487681)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_6 = Sketch_2.addLine(29.6987556208597, 11.4175813298688, 13.10955704685832, 11.4175813298688)
SketchLine_7 = Sketch_2.addLine(13.10955704685832, 11.4175813298688, 13.10955704685832, -4.852594194632538)
SketchLine_8 = Sketch_2.addLine(13.10955704685832, -4.852594194632538, 29.6987556208597, -4.852594194632538)
SketchLine_9 = Sketch_2.addLine(29.6987556208597, -4.852594194632538, 29.6987556208597, 11.4175813298688)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_6.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_7.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_9.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 10)
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_6r-SketchLine_7f-SketchLine_8f-SketchLine_9f")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("COMPSOLID", "Extrusion_1_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())

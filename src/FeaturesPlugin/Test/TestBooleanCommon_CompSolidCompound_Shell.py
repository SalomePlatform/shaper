# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(-21.66024096385543, 40.92621620645053, -77.63614457831326, 40.92621620645053)
SketchLine_2 = Sketch_1.addLine(-77.63614457831326, 40.92621620645053, -77.63614457831326, 1.907518733308514)
SketchLine_3 = Sketch_1.addLine(-77.63614457831326, 1.907518733308514, -21.66024096385543, 1.907518733308514)
SketchLine_4 = Sketch_1.addLine(-21.66024096385543, 1.907518733308514, -21.66024096385543, 40.92621620645053)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(65.46746987951809, 41.25025618940838, 10.46506024096384, 41.25025618940838)
SketchLine_6 = Sketch_1.addLine(10.46506024096384, 41.25025618940838, 10.46506024096384, -1.337003177360179)
SketchLine_7 = Sketch_1.addLine(10.46506024096384, -1.337003177360179, 65.46746987951809, -1.337003177360179)
SketchLine_8 = Sketch_1.addLine(65.46746987951809, -1.337003177360179, 65.46746987951809, 41.25025618940838)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(-54.27228915662651, 40.92621620645053, -55.24578313253012, 1.907518733308514)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_3.result())
SketchLine_10 = Sketch_1.addLine(31.88192771084336, 41.25025618940838, 30.42168674698797, -1.337003177360179)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchLine_5.result())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_7.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_11 = Sketch_2.addLine(19.22650602409638, 26.41583731352732, -12.83325439216414, 26.41583731352732)
SketchLine_12 = Sketch_2.addLine(-12.83325439216414, 26.41583731352732, -12.83325439216414, 12.07337391600091)
SketchLine_13 = Sketch_2.addLine(-12.83325439216414, 12.07337391600091, 19.22650602409638, 12.07337391600091)
SketchLine_14 = Sketch_2.addLine(19.22650602409638, 12.07337391600091, 19.22650602409638, 26.41583731352732)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_14.result())
SketchLine_15 = Sketch_2.addLine(0.2648302996693306, 26.41583731352732, -0.2234771832481639, 12.07337391600092)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_15.startPoint(), SketchLine_11.result())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_13.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 10)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_1_2")])
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_11r-SketchLine_15f-SketchLine_13f-SketchLine_14f"), model.selection("FACE", "Sketch_2/Face-SketchLine_11r-SketchLine_12f-SketchLine_13f-SketchLine_15r")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("COMPOUND", "Compound_1_1")])
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

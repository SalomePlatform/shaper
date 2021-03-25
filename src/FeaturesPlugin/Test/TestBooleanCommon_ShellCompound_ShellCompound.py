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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-11.4055110220441, 59.07236770836705, -90.54579158316633, 59.07236770836705)
SketchLine_2 = Sketch_1.addLine(-90.54579158316633, 59.07236770836705, -90.54579158316633, 18.29259905792451)
SketchLine_3 = Sketch_1.addLine(-90.54579158316633, 18.29259905792451, -11.4055110220441, 18.29259905792451)
SketchLine_4 = Sketch_1.addLine(-11.4055110220441, 18.29259905792451, -11.4055110220441, 59.07236770836705)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(116.15, 58.70699100906781, 29.5612224448898, 58.70699100906781)
SketchLine_6 = Sketch_1.addLine(29.5612224448898, 58.70699100906781, 29.5612224448898, 16.80919541788544)
SketchLine_7 = Sketch_1.addLine(29.5612224448898, 16.80919541788544, 116.15, 16.80919541788544)
SketchLine_8 = Sketch_1.addLine(116.15, 16.80919541788544, 116.15, 58.70699100906781)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(-51.44118236472945, 59.07236770836705, -50.51012024048097, 18.29259905792451)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_9.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_3.result())
SketchLine_10 = Sketch_1.addLine(72.85561122244489, 16.80919541788544, 73.32114228456915, 58.70699100906781)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.startPoint(), SketchLine_7.result())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_5.result())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_11 = Sketch_2.addLine(-33.65512410802471, 32.35440881763527, -66.62101906518004, 32.35440881763527)
SketchLine_12 = Sketch_2.addLine(-66.62101906518004, 32.35440881763527, -66.62101906518004, -10.93997995991984)
SketchLine_13 = Sketch_2.addLine(-66.62101906518004, -10.93997995991984, -33.65512410802471, -10.93997995991984)
SketchLine_14 = Sketch_2.addLine(-33.65512410802471, -10.93997995991984, -33.65512410802471, 32.35440881763527)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_14.result())
SketchLine_15 = Sketch_2.addLine(-66.62101906518004, 10.47444889779559, -33.6551241080247, 9.543386773547091)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_15.startPoint(), SketchLine_12.result())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_15.endPoint(), SketchLine_14.result())
SketchLine_16 = Sketch_2.addLine(86.07316931157348, 30.95781563126252, 62.43123950606181, 30.95781563126252)
SketchLine_17 = Sketch_2.addLine(62.43123950606181, 30.95781563126252, 62.43123950606181, -14.66422845691383)
SketchLine_18 = Sketch_2.addLine(62.43123950606181, -14.66422845691383, 86.07316931157348, -14.66422845691383)
SketchLine_19 = Sketch_2.addLine(86.07316931157348, -14.66422845691383, 86.07316931157348, 30.95781563126252)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_19.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintHorizontal_7 = Sketch_2.setHorizontal(SketchLine_16.result())
SketchConstraintVertical_7 = Sketch_2.setVertical(SketchLine_17.result())
SketchConstraintHorizontal_8 = Sketch_2.setHorizontal(SketchLine_18.result())
SketchConstraintVertical_8 = Sketch_2.setVertical(SketchLine_19.result())
SketchLine_20 = Sketch_2.addLine(62.43123950606181, 3.491482965931855, 86.07316931157347, 4.888076152304602)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_20.startPoint(), SketchLine_17.result())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_20.endPoint(), SketchLine_19.result())
model.do()
Shell_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_9r"), model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_9f-SketchLine_3f-SketchLine_4f"), model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_10f"), model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_10r-SketchLine_7f-SketchLine_8f")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Shell_2_objects = [model.selection("FACE", "Sketch_2/Face-SketchLine_11r-SketchLine_12f-SketchLine_15f-SketchLine_14f"), model.selection("FACE", "Sketch_2/Face-SketchLine_12f-SketchLine_13f-SketchLine_14f-SketchLine_15r"), model.selection("FACE", "Sketch_2/Face-SketchLine_16f-SketchLine_17f-SketchLine_20f-SketchLine_19f"), model.selection("FACE", "Sketch_2/Face-SketchLine_17f-SketchLine_18f-SketchLine_19f-SketchLine_20r")]
Shell_2 = model.addShell(Part_1_doc, Shell_2_objects)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("SHELL", "Shell_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_2_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [4])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [16])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [32])

assert(model.checkPythonDump())

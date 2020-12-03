# Copyright (C) 2020  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "r", "30")
model.addParameter(Part_1_doc, "nb", "8")
model.addParameter(Part_1_doc, "r2", "40")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchArc_1 = Sketch_1.addArc(7.762830395025333, 33.0188035531048, -7.554428638438673, 29.03326726268915, -2.687344813323973, 44.9056350037167, True)
SketchLine_2 = Sketch_1.addLine(-2.687344813323973, 44.9056350037167, 0, 44.9056350037167)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_1.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_1.result(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(-7.554428638438673, 29.03326726268915, -12.16952563119684, 27.83242339707059)
SketchLine_3.setAuxiliary(True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_3.startPoint())
SketchConstraintPerpendicular_2 = Sketch_1.setPerpendicular(SketchArc_1.results()[1], SketchLine_3.result())
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_1.result(), [SketchLine_2.result(), SketchArc_1.results()[1]])
[SketchLine_4, SketchArc_2] = SketchConstraintMirror_1.mirrored()
SketchArc_3 = Sketch_1.addArc(0, 0, -7.554428638438673, 29.03326726268915, -15.18783244321905, 25.87140787967874, False)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_3.results()[1], "r")
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_3.center(), SketchAPI_Line(SketchLine_1).startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_3.results()[1])
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_3.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchArc_3.startPoint())
SketchMultiRotation_1_objects = [SketchArc_1.results()[1], SketchLine_2.result(), SketchLine_4.result(), SketchArc_2.results()[1]]
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchAPI_Line(SketchLine_1).startPoint(), 360, "nb", True)
[SketchArc_4, SketchArc_5, SketchArc_6, SketchArc_7, SketchArc_8, SketchArc_9, SketchArc_10, SketchLine_5, SketchLine_6, SketchLine_7, SketchLine_8, SketchLine_9, SketchLine_10, SketchLine_11, SketchLine_12, SketchLine_13, SketchLine_14, SketchLine_15, SketchLine_16, SketchLine_17, SketchLine_18, SketchArc_11, SketchArc_12, SketchArc_13, SketchArc_14, SketchArc_15, SketchArc_16, SketchArc_17] = SketchMultiRotation_1.rotated()
SketchArc_13.setName("SketchArc_9")
SketchArc_13.result().setName("SketchArc_9")
SketchArc_13.results()[1].setName("SketchArc_9_2")
SketchArc_12.setName("SketchArc_8")
SketchArc_12.result().setName("SketchArc_8")
SketchArc_12.results()[1].setName("SketchArc_8_2")
SketchArc_11.setName("SketchArc_7")
SketchArc_11.result().setName("SketchArc_7")
SketchArc_11.results()[1].setName("SketchArc_7_2")
SketchLine_14.setName("SketchLine_10")
SketchLine_14.result().setName("SketchLine_10")
SketchLine_13.setName("SketchLine_9")
SketchLine_13.result().setName("SketchLine_9")
SketchLine_12.setName("SketchLine_8")
SketchLine_12.result().setName("SketchLine_8")
SketchLine_11.setName("SketchLine_14")
SketchLine_11.result().setName("SketchLine_14")
SketchLine_10.setName("SketchLine_13")
SketchLine_10.result().setName("SketchLine_13")
SketchLine_9.setName("SketchLine_12")
SketchLine_9.result().setName("SketchLine_12")
SketchLine_8.setName("SketchLine_11")
SketchLine_8.result().setName("SketchLine_11")
SketchArc_10.setName("SketchArc_13")
SketchArc_10.result().setName("SketchArc_13")
SketchArc_10.results()[1].setName("SketchArc_13_2")
SketchArc_9.setName("SketchArc_12")
SketchArc_9.result().setName("SketchArc_12")
SketchArc_9.results()[1].setName("SketchArc_12_2")
SketchArc_8.setName("SketchArc_11")
SketchArc_8.result().setName("SketchArc_11")
SketchArc_8.results()[1].setName("SketchArc_11_2")
SketchArc_7.setName("SketchArc_10")
SketchArc_7.result().setName("SketchArc_10")
SketchArc_7.results()[1].setName("SketchArc_10_2")
SketchArc_18 = Sketch_1.addArc(0, 0, -25.87140787967873, 15.18783244321905, -29.03326726268915, 7.554428638438678, False)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_3.results()[1], SketchArc_18.results()[1])
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_3.center(), SketchArc_18.center())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_18.startPoint(), SketchArc_4.results()[1])
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchArc_11.results()[1])
SketchArc_19 = Sketch_1.addArc(0, 0, 15.18783244321906, 25.87140787967872, 7.554428638438674, 29.03326726268915, False)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_19.endPoint(), SketchArc_2.results()[1])
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchArc_18.results()[1], SketchArc_19.results()[1])
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_18.center(), SketchArc_19.center())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_19.startPoint(), SketchArc_10.results()[1])
SketchArc_20 = Sketch_1.addArc(0, 0, -29.03326726268915, -7.55442863843867, -25.87140787967874, -15.18783244321904, False)
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchArc_18.results()[1], SketchArc_20.results()[1])
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_18.center(), SketchArc_20.center())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_20.startPoint(), SketchArc_5.results()[1])
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_18.endPoint(), SketchArc_12.results()[1])
SketchArc_21 = Sketch_1.addArc(0, 0, -15.18783244321905, -25.87140787967873, -7.554428638438704, -29.03326726268914, False)
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchArc_20.results()[1], SketchArc_21.results()[1])
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchArc_20.center(), SketchArc_21.center())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchArc_20.endPoint(), SketchArc_13.results()[1])
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchArc_21.startPoint(), SketchArc_6.results()[1])
SketchArc_22 = Sketch_1.addArc(0, 0, 25.87140787967873, -15.18783244321906, 29.03326726268915, -7.55442863843869, False)
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchArc_21.results()[1], SketchArc_22.results()[1])
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchArc_21.center(), SketchArc_22.center())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchArc_22.startPoint(), SketchArc_8.results()[1])
SketchArc_23 = Sketch_1.addArc(0, 0, 29.03326726268915, 7.554428638438663, 25.87140787967874, 15.18783244321903, False)
SketchConstraintCoincidence_21 = Sketch_1.setCoincident(SketchArc_23.endPoint(), SketchArc_17.results()[1])
SketchConstraintEqual_6 = Sketch_1.setEqual(SketchArc_22.results()[1], SketchArc_23.results()[1])
SketchConstraintCoincidence_22 = Sketch_1.setCoincident(SketchArc_22.center(), SketchArc_23.center())
SketchConstraintCoincidence_23 = Sketch_1.setCoincident(SketchArc_22.endPoint(), SketchArc_16.results()[1])
SketchConstraintCoincidence_24 = Sketch_1.setCoincident(SketchArc_23.startPoint(), SketchArc_9.results()[1])
SketchArc_24 = Sketch_1.addArc(0, 0, 7.554428638438642, -29.03326726268916, 15.18783244321904, -25.87140787967875, False)
SketchArc_24.setName("SketchArc_47")
SketchArc_24.result().setName("SketchArc_47")
SketchArc_24.results()[1].setName("SketchArc_47_2")
SketchConstraintCoincidence_25 = Sketch_1.setCoincident(SketchArc_24.endPoint(), SketchArc_15.results()[1])
SketchConstraintEqual_7 = Sketch_1.setEqual(SketchArc_21.results()[1], SketchArc_24.results()[1])
SketchConstraintCoincidence_26 = Sketch_1.setCoincident(SketchArc_21.center(), SketchArc_24.center())
SketchConstraintCoincidence_27 = Sketch_1.setCoincident(SketchArc_21.endPoint(), SketchArc_14.results()[1])
SketchConstraintCoincidence_28 = Sketch_1.setCoincident(SketchArc_24.startPoint(), SketchArc_7.results()[1])
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], False)

model.end()

from GeomAPI import *

model.testNbResults(Wire_1, 1)
model.testNbSubResults(Wire_1, [0])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.WIRE, [1])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.EDGE, [40])
model.testNbSubShapes(Wire_1, GeomAPI_Shape.VERTEX, [80])
model.testResultsVolumes(Wire_1, [0])

assert(model.checkPythonDump())

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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "r", "20")
model.addParameter(Part_1_doc, "l", "40")
model.addParameter(Part_1_doc, "r2", "3")
model.addParameter(Part_1_doc, "a", "10")
model.addParameter(Part_1_doc, "h", "5")
model.addParameter(Part_1_doc, "n", "2")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_3.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 20, 0, 0, 20, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.endPoint())
SketchArc_2 = Sketch_1.addArc(0, 0, 60, 0, 0, 60, False)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_2).startPoint(), SketchArc_2.center())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_2.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.endPoint())
SketchLine_3 = Sketch_1.addLine(0, 20, 0, 60)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_3.endPoint())
SketchLine_4 = Sketch_1.addLine(20, 0, 60, 0)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_4.endPoint())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), "l")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "r")
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", (30, 30, 0))])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE_2", (30, 30, 0)))
SketchCircle_1 = Sketch_2.addCircle(24.6201938253052, 4.341204441673258, 3)
SketchLine_5 = Sketch_2.addLine(24.6201938253052, 4.341204441673258, 54.16442641567144, 9.550649771681169)
SketchLine_5.setAuxiliary(True)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchLine_5.startPoint())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchCircle_1.results()[1], "r2")
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_4.createdFeature()
SketchProjection_5 = Sketch_2.addProjection(model.selection("VERTEX", (20, 0, 0)), False)
SketchPoint_2 = SketchProjection_5.createdFeature()
SketchMultiTranslation_1 = Sketch_2.addTranslation([SketchCircle_1.results()[1]], SketchCircle_1.center(), SketchLine_5.endPoint(), "n")
[SketchCircle_2] = SketchMultiTranslation_1.translated()
SketchConstraintLength_2 = Sketch_2.setLength(SketchLine_5.result(), "(l-2*h)/(n-1)")
SketchLine_7 = Sketch_2.addLine(0, 0, 24.6201938253052, 4.341204441673258)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_6).startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchCircle_1.center(), SketchLine_7.endPoint())
SketchConstraintCollinear_1 = Sketch_2.setCollinear(SketchLine_5.result(), SketchLine_7.result())
SketchConstraintAngle_1 = Sketch_2.setAngle(SketchLine_6.result(), SketchLine_7.result(), "a")
SketchPoint_3 = Sketch_2.addPoint(19.69615506024416, 3.472963553338606)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchLine_7.result())
SketchProjection_6 = Sketch_2.addProjection(model.selection("EDGE", (14.14213562373095, 14.14213562373095, 0)), False)
SketchArc_3 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchPoint_3.coordinates(), SketchArc_3.results()[1])
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchPoint_3.coordinates(), SketchLine_7.endPoint(), "h", True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 10, 0)
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_1")], model.selection("EDGE", "PartSet/OZ"), "(90-2*a)/(n-1)", "n")
Split_1 = model.addSplit(Part_1_doc, [model.selection("FACE", (30, 30, 0))], [model.selection("COMPOUND", (29.25281542867235, 29.25281542867235, 5))])
model.end()

assert(model.checkPythonDump())

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

# -*- coding: utf-8 -*-

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, -23.6272731038225, 7.84836794507966, 9.071298990348682, 0, 10, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.endPoint())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchArc_2 = Sketch_1.addArc(5.671059039631239, 0, 7.84836794507966, 9.071298990348682, 15, 0, True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.center())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_2.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchArc_2.endPoint(), 15, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchAPI_Line(SketchLine_2).startPoint(), SketchArc_1.endPoint(), 10, True)
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_2.result(), [SketchArc_2.results()[1], SketchArc_1.results()[1]])
[SketchArc_3, SketchArc_4] = SketchConstraintMirror_1.mirrored()
SketchConstraintMirror_2_objects = [SketchArc_1.results()[1], SketchArc_2.results()[1], SketchArc_3.results()[1], SketchArc_4.results()[1]]
SketchConstraintMirror_2 = Sketch_1.addMirror(SketchLine_1.result(), SketchConstraintMirror_2_objects)
[SketchArc_5, SketchArc_6, SketchArc_7, SketchArc_8] = SketchConstraintMirror_2.mirrored()
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 12, 12)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchLine_3 = Sketch_2.addLine(7.498934040825037, 4.111344571723496, -7.530337532973568, 4.111344571723496)
SketchLine_4 = Sketch_2.addLine(-7.530337532973568, 4.111344571723496, -7.530337532973568, -4.489390918751097)
SketchLine_5 = Sketch_2.addLine(-7.530337532973568, -4.489390918751097, 7.498934040825037, -4.489390918751097)
SketchLine_6 = Sketch_2.addLine(7.498934040825037, -4.489390918751097, 7.498934040825037, 4.111344571723496)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_6.result())
SketchCircle_1 = Sketch_2.addCircle(-0.133117930244568, 0.08984026389066591, 1.099112071963378)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_3r-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchCircle_1_2r")], model.selection(), model.selection(), 5, model.selection(), 10)
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_7 = Sketch_3.addLine(21.55890075100801, 30.21822825485694, -17.43219636481839, 31.63011070278906)
SketchLine_8 = Sketch_3.addLine(-17.43219636481839, 31.63011070278906, -12.33809712604348, -45.93749869468675)
SketchConstraintCoincidence_10 = Sketch_3.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_3.addLine(-12.33809712604348, -45.93749869468675, 30.10255255530162, -25.9120009302625)
SketchConstraintCoincidence_11 = Sketch_3.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_3.addLine(30.10255255530162, -25.9120009302625, 21.55890075100801, 30.21822825485694)
SketchConstraintCoincidence_12 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_7.startPoint(), SketchLine_10.endPoint())
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_3/SketchLine_7"), model.selection("EDGE", "Sketch_3/SketchLine_8"), model.selection("EDGE", "Sketch_3/SketchLine_9"), model.selection("EDGE", "Sketch_3/SketchLine_10")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Partition_1_objects = [model.selection("FACE", "Face_1_1"), model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()

model.checkResult(Partition_1,model,1,[2],[4],[42],[198],[396])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

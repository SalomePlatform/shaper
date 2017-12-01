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

"""
    Verify the order of sketch edges is the same after dump
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(40, 5, 40, -25)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 30)
SketchLine_2 = Sketch_1.addLine(40, -25, -10, -25)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 50)
SketchLine_3 = Sketch_1.addLine(-10, -25, -10, 5)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-10, 5, 40, 5)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
model.do()

# Change Length references
SketchConstraintLength_1.feature().refattr("ConstraintEntityA").setObject(SketchLine_3.feature().lastResult())
SketchConstraintLength_2.feature().refattr("ConstraintEntityA").setObject(SketchLine_4.feature().lastResult())
model.do()
# Remove the first line, then build it and constraints from scratch
Part_1_doc.removeFeature(SketchConstraintCoincidence_1.feature())
Part_1_doc.removeFeature(SketchConstraintCoincidence_4.feature())
Part_1_doc.removeFeature(SketchLine_1.feature())
SketchLine_1 = Sketch_1.addLine(40, 5, 40, -25)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
Sketch_1.setLength(SketchLine_1.result(), 20)
Sketch_1.setLength(SketchLine_2.result(), 40)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2r-SketchLine_3r-SketchLine_4r")], model.selection(), 10, 0)

# Extrude all lateral faces to check their area
Extrusion_2 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_1"))
SketchLine_5 = Sketch_2.addLine(0.7346748749771982, 2.736245541082907e-015, 0.7346748749771982, 10.00000000000001)
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchPoint_1.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchPoint_2.result())
SketchLine_6 = Sketch_2.addLine(0.7346748749771982, 10.00000000000001, -19.2653251250228, 10.00000000000001)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchPoint_3.result())
SketchLine_7 = Sketch_2.addLine(-19.2653251250228, 10.00000000000001, -19.2653251250228, 2.775557561562891e-015)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"), False)
SketchPoint_4 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchPoint_4.result())
SketchLine_8 = Sketch_2.addLine(-19.2653251250228, 2.775557561562891e-015, 0.7346748749771974, 3.386180225106727e-015)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_8.endPoint())
Extrusion_2.setNestedSketch(Sketch_2)

Extrusion_3 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_2"))
SketchLine_9 = Sketch_3.addLine(36.86324678550901, 2.286454635368208e-015, 36.86324678550901, 10)
SketchProjection_5 = Sketch_3.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"), False)
SketchPoint_5 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_9.startPoint(), SketchPoint_5.result())
SketchProjection_6 = Sketch_3.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"), False)
SketchPoint_6 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchPoint_6.result())
SketchLine_10 = Sketch_3.addLine(36.86324678550901, 10, -3.136753214490995, 10)
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchProjection_7 = Sketch_3.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"), False)
SketchPoint_7 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchPoint_7.result())
SketchLine_11 = Sketch_3.addLine(-3.136753214490995, 10, -3.136753214490995, 2.275957200481571e-015)
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchProjection_8 = Sketch_3.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_1"), False)
SketchPoint_8 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchPoint_8.result())
SketchLine_12 = Sketch_3.addLine(-3.136753214490995, 2.275957200481571e-015, 36.86324678550902, 1.387778780781446e-015)
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_9.startPoint(), SketchLine_12.endPoint())
Extrusion_3.setNestedSketch(Sketch_3)

Extrusion_4 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_4 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_3"))
SketchLine_13 = Sketch_4.addLine(22.60959895285982, 4.420942808558057e-016, 22.60959895285982, 10)
SketchProjection_9 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/From_Face_1"), False)
SketchPoint_9 = SketchProjection_9.createdFeature()
SketchConstraintCoincidence_21 = Sketch_4.setCoincident(SketchLine_13.startPoint(), SketchPoint_9.result())
SketchProjection_10 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"), False)
SketchPoint_10 = SketchProjection_10.createdFeature()
SketchConstraintCoincidence_22 = Sketch_4.setCoincident(SketchLine_13.endPoint(), SketchPoint_10.result())
SketchLine_14 = Sketch_4.addLine(22.60959895285982, 10, -7.390401047140179, 10)
SketchConstraintCoincidence_23 = Sketch_4.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchProjection_11 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1"), False)
SketchPoint_11 = SketchProjection_11.createdFeature()
SketchConstraintCoincidence_24 = Sketch_4.setCoincident(SketchLine_14.endPoint(), SketchPoint_11.result())
SketchLine_15 = Sketch_4.addLine(-7.390401047140179, 10, -7.390401047140179, 8.881784197001252e-016)
SketchConstraintCoincidence_25 = Sketch_4.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchProjection_12 = Sketch_4.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_1"), False)
SketchPoint_12 = SketchProjection_12.createdFeature()
SketchConstraintCoincidence_26 = Sketch_4.setCoincident(SketchLine_15.endPoint(), SketchPoint_12.result())
SketchLine_16 = Sketch_4.addLine(-7.390401047140179, 8.881784197001252e-016, 22.60959895285983, 0)
SketchConstraintCoincidence_27 = Sketch_4.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_28 = Sketch_4.setCoincident(SketchLine_13.startPoint(), SketchLine_16.endPoint())
Extrusion_4.setNestedSketch(Sketch_4)

Extrusion_5 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_5 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face_4"))
SketchLine_17 = Sketch_5.addLine(-10.64977324988603, -1.185841873934692e-016, -10.64977324988603, -10)
SketchProjection_13 = Sketch_5.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/From_Face_1"), False)
SketchPoint_13 = SketchProjection_13.createdFeature()
SketchConstraintCoincidence_29 = Sketch_5.setCoincident(SketchLine_17.startPoint(), SketchPoint_13.result())
SketchProjection_14 = Sketch_5.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1"), False)
SketchPoint_14 = SketchProjection_14.createdFeature()
SketchConstraintCoincidence_30 = Sketch_5.setCoincident(SketchLine_17.endPoint(), SketchPoint_14.result())
SketchLine_18 = Sketch_5.addLine(-10.64977324988603, -10, 39.35022675011398, -10)
SketchConstraintCoincidence_31 = Sketch_5.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchProjection_15 = Sketch_5.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1"), False)
SketchPoint_15 = SketchProjection_15.createdFeature()
SketchConstraintCoincidence_32 = Sketch_5.setCoincident(SketchLine_18.endPoint(), SketchPoint_15.result())
SketchLine_19 = Sketch_5.addLine(39.35022675011398, -10, 39.35022675011398, 4.440892098500626e-016)
SketchConstraintCoincidence_33 = Sketch_5.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchProjection_16 = Sketch_5.addProjection(model.selection("VERTEX", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"), False)
SketchPoint_16 = SketchProjection_16.createdFeature()
SketchConstraintCoincidence_34 = Sketch_5.setCoincident(SketchLine_19.endPoint(), SketchPoint_16.result())
SketchLine_20 = Sketch_5.addLine(39.35022675011398, 4.440892098500626e-016, -10.64977324988603, 0)
SketchConstraintCoincidence_35 = Sketch_5.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_36 = Sketch_5.setCoincident(SketchLine_17.startPoint(), SketchLine_20.endPoint())
Extrusion_5.setNestedSketch(Sketch_5)
model.do()
model.end()

# check volumes
model.testResultsVolumes(Extrusion_2, [4000])
model.testResultsVolumes(Extrusion_3, [3000])
model.testResultsVolumes(Extrusion_4, [5000])
model.testResultsVolumes(Extrusion_5, [2000])

assert(model.checkPythonDump())

# check volumes after dump and restore
model.testResultsVolumes(Extrusion_2, [4000])
model.testResultsVolumes(Extrusion_3, [3000])
model.testResultsVolumes(Extrusion_4, [5000])
model.testResultsVolumes(Extrusion_5, [2000])

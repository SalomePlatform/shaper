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

# The test for correct selection of a whole feautre as argument of other feature: select all extrusions
# produced from the scetch, then change the number of sketch contours, so, anytway all extrusions must
# still be selected in the Cut Boolean oepration

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "Ray", "12.5")
model.addParameter(Part_1_doc, "Lgx", "50")
model.addParameter(Part_1_doc, "Lgy", "60")
model.addParameter(Part_1_doc, "Lgz", "75")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(0, 0, 0, 75)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchPoint_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.result())
SketchLine_3 = Sketch_1.addLine(0, 75, 37.5, 75)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
SketchArc_1 = Sketch_1.addArc(37.5, 62.5, 37.5, 75, 50, 62.5, True)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchLine_4 = Sketch_1.addLine(50, 62.5, 50, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_4.startPoint())
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_5 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.result())
SketchLine_6 = Sketch_1.addLine(50, 0, 0, 0)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_1.results()[1])
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_4.result(), SketchArc_1.results()[1])
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "Lgz")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_6.result(), "Lgx")
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], "Ray")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_4r-SketchArc_1_2f-SketchLine_3r-SketchLine_1r")], model.selection(), 0, "Lgy")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchLine_7 = Sketch_2.addLine(0, -75, 0, 0)
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), False)
SketchPoint_2 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchPoint_2.result())
SketchProjection_5 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/To_Face]"), False)
SketchPoint_3 = SketchProjection_5.createdFeature()
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchPoint_3.result())
SketchLine_8 = Sketch_2.addLine(0, 0, 60, 0)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchProjection_6 = Sketch_2.addProjection(model.selection("VERTEX", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_6][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/From_Face]"), False)
SketchPoint_4 = SketchProjection_6.createdFeature()
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchPoint_4.result())
SketchLine_9 = Sketch_2.addLine(60, 0, 59.99999999999999, -62.5)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchProjection_7 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/From_Face]"), False)
SketchLine_10 = SketchProjection_7.createdFeature()
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.result())
SketchLine_11 = Sketch_2.addLine(0, -75, 47.49999999999999, -75)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchLine_11.startPoint())
SketchProjection_8 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1]"), False)
SketchLine_12 = SketchProjection_8.createdFeature()
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.result())
SketchArc_2 = Sketch_2.addArc(47.49999999999999, -62.5, 59.99999999999999, -62.5, 47.49999999999999, -75, True)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchArc_2.startPoint())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchArc_2.results()[1], "Ray")
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchLine_11.endPoint())
SketchConstraintTangent_3 = Sketch_2.setTangent(SketchArc_2.results()[1], SketchLine_11.result())
SketchConstraintTangent_4 = Sketch_2.setTangent(SketchLine_9.result(), SketchArc_2.results()[1])
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_11f-SketchArc_2_2f-SketchLine_9r-SketchLine_8r-SketchLine_7r")], model.selection(), 0, "Lgx")
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1")])
model.do()
model.end()

assert(len(Common_1.results()) == 1)
model.testResultsVolumes(Common_1, [221498.777690970688126981258392334])

assert(model.checkPythonDump())

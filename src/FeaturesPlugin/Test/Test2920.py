# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-28.02134146341463, 30.17682926829269, -40.03048780487805, 30.17682926829269)
SketchLine_2 = Sketch_1.addLine(-40.03048780487805, 30.17682926829269, -40.03048780487805, 21.24695121951219)
SketchLine_3 = Sketch_1.addLine(-40.03048780487805, 21.24695121951219, -28.02134146341463, 21.24695121951219)
SketchLine_4 = Sketch_1.addLine(-28.02134146341463, 21.24695121951219, -28.02134146341463, 30.17682926829269)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-24.32621951219512, 13.39481707317073, -38.79878048780488, 13.39481707317073)
SketchLine_6 = Sketch_1.addLine(-38.79878048780488, 13.39481707317073, -38.79878048780488, 9.545731707317071)
SketchLine_7 = Sketch_1.addLine(-38.79878048780488, 9.545731707317071, -24.32621951219512, 9.545731707317071)
SketchLine_8 = Sketch_1.addLine(-24.32621951219512, 9.545731707317071, -24.32621951219512, 13.39481707317073)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchCircle_1 = Sketch_1.addCircle(-15.39634146341463, 19.39939024390243, 5.234756097560973)
Extrusion_1.setNestedSketch(Sketch_1)
Extrusion_2 = model.addExtrusion(Part_1_doc, [], model.selection(), 10, 10)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchEllipse_1 = Sketch_2.addEllipse(9.45504630186378, 35.70277888541424, 12.45818659778603, 26.02599348744252, 6.842520574992792)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_9, SketchLine_10] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
SketchLine_11 = Sketch_2.addLine(18.91725801191108, 15.97306085169858, 9.656369955269037, 15.97306085169858)
SketchLine_12 = Sketch_2.addLine(9.656369955269037, 15.97306085169858, 9.656369955269037, -4.360628142232864)
SketchLine_13 = Sketch_2.addLine(9.656369955269037, -4.360628142232864, 18.91725801191108, -4.360628142232864)
SketchLine_14 = Sketch_2.addLine(18.91725801191108, -4.360628142232864, 18.91725801191108, 15.97306085169858)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_14.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_12.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_14.result())
SketchCircle_2 = Sketch_2.addCircle(-32.8229209132412, -2.146067954774983, 6.582390686251648)
Extrusion_2.setNestedSketch(Sketch_2)
Compound_1_objects = [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2"), model.selection("SOLID", "Extrusion_1_3")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
Compound_1.result().setName("A")
Compound_1.result().setColor(0, 255, 0)
Compound_1.result().subResult(0).setColor(0, 255, 0)
Compound_1.result().subResult(1).setColor(0, 255, 0)
Compound_1.result().subResult(2).setColor(0, 255, 0)
Compound_2_objects = [model.selection("SOLID", "Extrusion_2_1"), model.selection("SOLID", "Extrusion_2_2"), model.selection("SOLID", "Extrusion_2_3")]
Compound_2 = model.addCompound(Part_1_doc, Compound_2_objects)
Compound_2.result().setName("B")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Compound_1_1_1/Modified_Face&Extrusion_1_1/To_Face"), 5, True)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_15 = Sketch_3.addLine(-43.21451225862969, 24.76425132139253, -23.11145813617009, 27.98073998098607)
SketchLine_16 = Sketch_3.addLine(-23.11145813617009, 27.98073998098607, 25.13587175773296, 39.84154191323723)
SketchConstraintCoincidence_13 = Sketch_3.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchLine_17 = Sketch_3.addLine(25.13587175773296, 39.84154191323723, 22.9245358042624, 1.746254351176285)
SketchConstraintCoincidence_14 = Sketch_3.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_3.addLine(22.9245358042624, 1.746254351176285, -48.84336741291839, 6.973048423015779)
SketchConstraintCoincidence_15 = Sketch_3.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_3.addLine(-48.84336741291839, 6.973048423015779, -43.21451225862969, 24.76425132139253)
SketchConstraintCoincidence_16 = Sketch_3.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_17 = Sketch_3.setCoincident(SketchLine_15.startPoint(), SketchLine_19.endPoint())
SketchLine_20 = Sketch_3.addLine(-42.51090536434361, 23.45755280343266, -22.9104275949455, 26.1714651099647)
SketchLine_21 = Sketch_3.addLine(-22.9104275949455, 26.1714651099647, 23.32659688671159, 37.73072123037897)
SketchConstraintCoincidence_18 = Sketch_3.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchLine_22 = Sketch_3.addLine(23.32659688671159, 37.73072123037897, 20.91423039201644, 3.957590304646839)
SketchConstraintCoincidence_19 = Sketch_3.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())
SketchLine_23 = Sketch_3.addLine(20.91423039201644, 3.957590304646839, -46.02893983577404, 7.97820112913876)
SketchConstraintCoincidence_20 = Sketch_3.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
SketchLine_24 = Sketch_3.addLine(-46.02893983577404, 7.97820112913876, -42.51090536434361, 23.45755280343266)
SketchConstraintCoincidence_21 = Sketch_3.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
SketchConstraintCoincidence_22 = Sketch_3.setCoincident(SketchLine_20.startPoint(), SketchLine_24.endPoint())
model.do()
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchLine_19r-SketchLine_18r-SketchLine_17r-SketchLine_16r-SketchLine_15r-SketchLine_20f-SketchLine_21f-SketchLine_22f-SketchLine_23f-SketchLine_24f")], model.selection(), 10, 0)
Extrusion_3.result().setName("C")
Extrusion_3.result().setColor(0, 85, 255)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Compound_1_1_1"), model.selection("SOLID", "Compound_2_1_2")], [model.selection("SOLID", "C")], keepSubResults = True)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [6])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [35])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [138])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [276])
model.testResultsVolumes(Cut_1, [14047.4641487433])

assert(model.checkPythonDump())

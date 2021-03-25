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
SketchLine_1 = Sketch_1.addLine(-20.13803680981595, 73.73619631901842, -73.42638036809818, 73.73619631901842)
SketchLine_2 = Sketch_1.addLine(-73.42638036809818, 73.73619631901842, -73.42638036809818, 32.22085889570552)
SketchLine_3 = Sketch_1.addLine(-73.42638036809818, 32.22085889570552, -20.13803680981595, 32.22085889570552)
SketchLine_4 = Sketch_1.addLine(-20.13803680981595, 32.22085889570552, -20.13803680981595, 73.73619631901842)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(82.41104294478531, 71.56748466257669, 24.47546012269937, 71.56748466257669)
SketchLine_6 = Sketch_1.addLine(24.47546012269937, 71.56748466257669, 24.47546012269937, 17.96932515337424)
SketchLine_7 = Sketch_1.addLine(24.47546012269937, 17.96932515337424, 82.41104294478531, 17.96932515337424)
SketchLine_8 = Sketch_1.addLine(82.41104294478531, 17.96932515337424, 82.41104294478531, 71.56748466257669)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
SketchLine_9 = Sketch_1.addLine(-0.3098159509202501, -18.89877300613497, -69.70858895705524, -18.89877300613497)
SketchLine_10 = Sketch_1.addLine(-69.70858895705524, -18.89877300613497, -69.70858895705524, -72.80674846625769)
SketchLine_11 = Sketch_1.addLine(-69.70858895705524, -72.80674846625769, -0.3098159509202501, -72.80674846625769)
SketchLine_12 = Sketch_1.addLine(-0.3098159509202501, -72.80674846625769, -0.3098159509202501, -18.89877300613497)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_1.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_1.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_1.setVertical(SketchLine_12.result())
SketchLine_13 = Sketch_1.addLine(74.04601226993866, -25.09509202453988, 29.74233128834356, -25.09509202453988)
SketchLine_14 = Sketch_1.addLine(29.74233128834356, -25.09509202453988, 29.74233128834356, -77.45398773006136)
SketchLine_15 = Sketch_1.addLine(29.74233128834356, -77.45398773006136, 74.04601226993866, -77.45398773006136)
SketchLine_16 = Sketch_1.addLine(74.04601226993866, -77.45398773006136, 74.04601226993866, -25.09509202453988)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_16.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintHorizontal_7 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintVertical_7 = Sketch_1.setVertical(SketchLine_14.result())
SketchConstraintHorizontal_8 = Sketch_1.setHorizontal(SketchLine_15.result())
SketchConstraintVertical_8 = Sketch_1.setVertical(SketchLine_16.result())
SketchLine_17 = Sketch_1.addLine(52.66871165644174, 52.66871165644174, -42.75460122699386, 52.66871165644174)
SketchLine_18 = Sketch_1.addLine(-42.75460122699386, 52.66871165644174, -42.75460122699386, -41.51533742331289)
SketchLine_19 = Sketch_1.addLine(-42.75460122699386, -41.51533742331289, 52.66871165644174, -41.51533742331289)
SketchLine_20 = Sketch_1.addLine(52.66871165644174, -41.51533742331289, 52.66871165644174, 52.66871165644174)
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_20.endPoint(), SketchLine_17.startPoint())
SketchConstraintCoincidence_18 = Sketch_1.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_19 = Sketch_1.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_20 = Sketch_1.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())
SketchConstraintHorizontal_9 = Sketch_1.setHorizontal(SketchLine_17.result())
SketchConstraintVertical_9 = Sketch_1.setVertical(SketchLine_18.result())
SketchConstraintHorizontal_10 = Sketch_1.setHorizontal(SketchLine_19.result())
SketchConstraintVertical_10 = Sketch_1.setVertical(SketchLine_20.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 100, 100)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1")], model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_20][Extrusion_1_1_2/To_Face]"), 200)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 50, 50)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_2_1")], model.selection("EDGE", "[Extrusion_2_1_2/Generated_Face&Sketch_1/SketchLine_20][Extrusion_2_1_2/To_Face]"), 150)
Boolean_1 = model.addCut(Part_1_doc, [model.selection("COMPSOLID", "Translation_1_1")], [model.selection("COMPSOLID", "Translation_2_1")])
model.do()
model.testHaveNamingSubshapes(Boolean_1, model, Part_1_doc)
model.end()

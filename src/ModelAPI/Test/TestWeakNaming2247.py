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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 0, 85.6495043418947, 0, -85.6495043418947, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(0, -85.6495043418947, 0, 85.6495043418947)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.endPoint())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchLine_3 = Sketch_2.addLine(42.96245524296674, 29.66455242966749, -38.35933503836317, 29.66455242966749)
SketchLine_4 = Sketch_2.addLine(-38.35933503836317, 29.66455242966749, -38.35933503836317, -29.66455242966754)
SketchLine_5 = Sketch_2.addLine(-38.35933503836317, -29.66455242966754, 42.96245524296674, -29.66455242966754)
SketchLine_6 = Sketch_2.addLine(42.96245524296674, -29.66455242966754, 42.96245524296674, 29.66455242966749)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_6.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 100, 100)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Revolution_1_1")], [model.selection("SOLID", "Extrusion_1_1")])
model.do()
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.end()

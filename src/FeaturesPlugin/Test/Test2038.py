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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-379.073756432247, 108.9193825042881, -672.3842195540308, -33.44768439108059)
SketchLine_2 = Sketch_1.addLine(-672.3842195540308, -33.44768439108059, -360.2058319039451, -146.6552315608919)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-360.2058319039451, -146.6552315608919, -78.9022298456261, -14.57975986277871)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-78.9022298456261, -14.57975986277871, -379.073756432247, 108.9193825042881)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-379.073756432247, 108.9193825042881, -360.2058319039451, -146.6552315608919)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_5r")], model.selection(), 100, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3f-SketchLine_4f-SketchLine_5f")], model.selection(), 100, 0)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OX"), 50)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Extrusion_2_1")])
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Partition_1_1")], model.selection("EDGE", "PartSet/OZ"), 50)
#Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Translation_2_1/Translated_Face_24"), model.selection("FACE", "Translation_2_1/Translated_Face_13"), model.selection("FACE", "Translation_2_1/Translated_Face_16"), model.selection("FACE", "Translation_2_1/Translated_Face_12"), model.selection("FACE", "Translation_2_1/Translated_Face_9"), model.selection("FACE", "Translation_2_1/Translated_Face_10"), model.selection("FACE", "Translation_2_1/Translated_Face_11"), model.selection("FACE", "Translation_2_1/Translated_Face_25"), model.selection("FACE", "Translation_2_1/Translated_Face_20"), model.selection("FACE", "Translation_2_1/Translated_Face_19"), model.selection("FACE", "Translation_2_1/Translated_Face_18"), model.selection("FACE", "Translation_2_1/Translated_Face_27"), model.selection("FACE", "Translation_2_1/Translated_Face_17"), model.selection("FACE", "Translation_2_1/Translated_Face_31"), model.selection("FACE", "Translation_2_1/Translated_Face_32"), model.selection("FACE", "Translation_2_1/Translated_Face_30"), model.selection("FACE", "Translation_2_1/Translated_Face_28"), model.selection("FACE", "Translation_2_1/Translated_Face_29"), model.selection("FACE", "Translation_2_1/Translated_Face_8"), model.selection("FACE", "Translation_2_1/Translated_Face_22"), model.selection("FACE", "Translation_2_1/Translated_Face_7"), model.selection("FACE", "Translation_2_1/Translated_Face_6"), model.selection("FACE", "Translation_2_1/Translated_Face_5"), model.selection("FACE", "Translation_2_1/Translated_Face_3"), model.selection("FACE", "Translation_2_1/Translated_Face_1"), model.selection("FACE", "Translation_2_1/Translated_Face_2"), model.selection("FACE", "Translation_2_1/Translated_Face_21"), model.selection("FACE", "Translation_2_1/Translated_Face_4"), model.selection("FACE", "Translation_2_1/Translated_Face_25"), model.selection("FACE", "Translation_2_1/Translated_Face_27"), model.selection("FACE", "Translation_2_1/Translated_Face_21"), model.selection("FACE", "Translation_2_1/Translated_Face_22"), model.selection("FACE", "Translation_2_1/Translated_Face_26"), model.selection("FACE", "Translation_2_1/Translated_Face_23"), model.selection("FACE", "Translation_2_1/Translated_Face_30"), model.selection("FACE", "Translation_2_1/Translated_Face_23"), model.selection("FACE", "Translation_2_1/Translated_Face_14"), model.selection("FACE", "Translation_2_1/Translated_Face_15")])
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Translation_2_1_6/Translated_Face_1"), model.selection("FACE", "Translation_2_1_5/Translated_Face_1"), model.selection("FACE", "Translation_2_1_5/Translated_Face_4"), model.selection("FACE", "Translation_2_1_4/Translated_Face_4"), model.selection("FACE", "Translation_2_1_5/Translated_Face_3"), model.selection("FACE", "Translation_2_1_5/Translated_Face_2"), model.selection("FACE", "Translation_2_1_5/Translated_Face_5"), model.selection("FACE", "Translation_2_1_7/Translated_Face_5"), model.selection("FACE", "Translation_2_1_6/Translated_Face_5"), model.selection("FACE", "Translation_2_1_4/Translated_Face_1"), model.selection("FACE", "Translation_2_1_2/Translated_Face_2"), model.selection("FACE", "Translation_2_1_5/Translated_Face_5"), model.selection("FACE", "Translation_2_1_2/Translated_Face_1"), model.selection("FACE", "Translation_2_1_4/Translated_Face_5"), model.selection("FACE", "Translation_2_1_2/Translated_Face_3"), model.selection("FACE", "Translation_2_1_3/Translated_Face_5"), model.selection("FACE", "Translation_2_1_1/Translated_Face_3"), model.selection("FACE", "Translation_2_1_4/Translated_Face_2"), model.selection("FACE", "Translation_2_1_2/Translated_Face_3"), model.selection("FACE", "Translation_2_1_1/Translated_Face_4"), model.selection("FACE", "Translation_2_1_1/Translated_Face_1"), model.selection("FACE", "Translation_2_1_1/Translated_Face_2"), model.selection("FACE", "Translation_2_1_3/Translated_Face_4"), model.selection("FACE", "Translation_2_1_4/Translated_Face_3"), model.selection("FACE", "Translation_2_1_3/Translated_Face_5"), model.selection("FACE", "Translation_2_1_3/Translated_Face_2"), model.selection("FACE", "Translation_2_1_3/Translated_Face_3"), model.selection("FACE", "Translation_2_1_3/Translated_Face_1"), model.selection("FACE", "Translation_2_1_7/Translated_Face_3"), model.selection("FACE", "Translation_2_1_7/Translated_Face_5"), model.selection("FACE", "Translation_2_1_7/Translated_Face_4"), model.selection("FACE", "Translation_2_1_7/Translated_Face_1"), model.selection("FACE", "Translation_2_1_7/Translated_Face_2"), model.selection("FACE", "Translation_2_1_6/Translated_Face_4"), model.selection("FACE", "Translation_2_1_4/Translated_Face_5"), model.selection("FACE", "Translation_2_1_6/Translated_Face_5"), model.selection("FACE", "Translation_2_1_6/Translated_Face_2"), model.selection("FACE", "Translation_2_1_6/Translated_Face_3")])

model.end()

groupFeature = Group_1.feature()
groupSelectionList = groupFeature.selectionList("group_list")
for index in range(0, groupSelectionList.size()):
  attrSelection = groupSelectionList.value(index)
  shape = attrSelection.value()
  name = attrSelection.namingName()
  assert(shape.isFace())
  assert(name != ""), "Empty shape name"

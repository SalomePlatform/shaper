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
SketchLine_1 = Sketch_1.addLine(0, 50, 100, 50)
SketchLine_2 = Sketch_1.addLine(100, 50, 100, 0)
SketchLine_3 = Sketch_1.addLine(100, 0, 0, 0)
SketchLine_4 = Sketch_1.addLine(0, 0, 0, 50)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(50, 50, 50, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_1.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_3.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1r-SketchLine_3r-SketchLine_4r-SketchLine_5r")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1r-SketchLine_2r-SketchLine_3r-SketchLine_5f")])
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OY"), 100)
Group_1 = model.addGroup(Part_1_doc, [model.selection("EDGE", "Translation_1_1/Translated_Edge_1"), model.selection("EDGE", "Translation_1_1/Translated_Edge_2"), model.selection("EDGE", "Translation_1_1/Translated_Edge_3"), model.selection("EDGE", "Translation_1_1/Translated_Edge_4"), model.selection("EDGE", "Translation_1_2/Translated_Edge_1"), model.selection("EDGE", "Translation_1_2/Translated_Edge_2"), model.selection("EDGE", "Translation_1_2/Translated_Edge_3"), model.selection("EDGE", "Translation_1_2/Translated_Edge_4")])
aGroupFeature = Group_1.feature()
aSelectionList = aGroupFeature.selectionList("group_list")
model.end()
assert(aSelectionList.size() == 8)
for index in range(0, aSelectionList.size()):
    attrSelection = aSelectionList.value(index)
    shape = attrSelection.value()
    name = attrSelection.namingName()
    assert(shape.isEdge())
    assert(name)

assert(model.checkPythonDump())
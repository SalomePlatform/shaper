from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-1154.716981132076, -199.5283018867925, -849.0566037735849, 128.7735849056603)
SketchLine_2 = Sketch_1.addLine(-849.0566037735849, 128.7735849056603, -563.2075471698113, -69.33962264150945)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-563.2075471698113, -69.33962264150945, -70.75471698113215, 326.8867924528301)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "PartSet/Sketch_1/Edge-SketchLine_1"), model.selection("EDGE", "PartSet/Sketch_1/Edge-SketchLine_2"), model.selection("EDGE", "PartSet/Sketch_1/Edge-SketchLine_3")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 100, 0)
Group_1 = model.addGroup(Part_1_doc, [model.selection("FACE", "Extrusion_1_1/Generated_Face_3"), model.selection("FACE", "Extrusion_1_1/Generated_Face_2"), model.selection("FACE", "Extrusion_1_1/Generated_Face_1")])
model.do()
model.end()

groupFeature = Group_1.feature()
groupSelectionList = groupFeature.selectionList("group_list")
assert(groupSelectionList.size() == 3)
for index in range(0, groupSelectionList.size()):
  attrSelection = groupSelectionList.value(index)
  shape = attrSelection.value()
  name = attrSelection.namingName()
  assert(shape.isFace())
  assert(name != ""), "Empty shape name"

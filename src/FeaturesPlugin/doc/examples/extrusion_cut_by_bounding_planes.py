from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc,
                                       [],
                                       model.selection(),
                                       model.selection("FACE", "Box_1_1/Bottom"),
                                       0,
                                       model.selection("FACE", "Box_1_1/Top"),
                                       0,
                                       [model.selection("SOLID", "Box_1_1")])
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(5, 0, 2)
ExtrusionCut_1.setNestedSketch(Sketch_1)
model.do()
model.end()

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 50)
model.do()
Plane_4 = model.addPlane(Part_1_doc,
                         model.selection("FACE", "PartSet/XOY"),
                         model.selection("EDGE", "PartSet/OX"),
                         45)
Plane_5 = model.addPlane(Part_1_doc,
                        model.selection("FACE", "PartSet/XOY"),
                        model.selection("EDGE", "PartSet/OX"),
                        315)
Extrusion_1 = model.addExtrusion(Part_1_doc,
                                 [model.selection("COMPOUND", "Sketch_1")],
                                 model.selection(),
                                 model.selection("FACE", "Plane_2"),
                                 50,
                                 model.selection("FACE", "Plane_1"),
                                 50)
model.do()
model.end()

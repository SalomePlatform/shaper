from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Objects to be cut
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Box_3 = model.addBox(Part_1_doc, 20, 20, 20)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], 20, 10, 0)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], 40, 20, 0)
ExtrusionFuse_1_objects_2 = [model.selection("SOLID", "Box_1_1"),
                             model.selection("SOLID", "Translation_1_1"),
                             model.selection("SOLID", "Translation_2_1")]

# Extrusion direction
Point_1 = model.addPoint(Part_1_doc, 0, 0, 0)
Point_2 = model.addPoint(Part_1_doc, 60, 40, 20)
Edge_1 = model.addEdge(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))

# Base object for extrusion
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Back"))
SketchCircle_1 = Sketch_1.addCircle(2.13, -2.43, 1.6)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")])

# Extrusion fuse through all
ExtrusionFuse_1 = model.addExtrusionFuse(Part_1_doc,
                                         [model.selection("FACE", "Face_1_1")],
                                         model.selection("EDGE", "Edge_1_1"),
                                         ExtrusionFuse_1_objects_2)

model.do()
model.end()

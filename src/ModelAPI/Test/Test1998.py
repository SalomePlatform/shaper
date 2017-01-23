from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-106.3464837049742, -78.0445969125214, -108.0617495711835, 165.5231560891939)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_1")])
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1e"), model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1s")])
model.end()

assert(model.checkPythonDump())

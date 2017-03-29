"""
Test case for Primitive Box feature. 
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Init
Point_1 = model.addPoint(Part_1_doc, 0, 0, 0).result()
Point_2 = model.addPoint(Part_1_doc, 50, 50, 50).result()
Point_3 = model.addPoint(Part_1_doc, 0, 50, 50).result()
Point_4 = model.addPoint(Part_1_doc, 50, 0, 50).result()
Point_5 = model.addPoint(Part_1_doc, 50, 50, 0).result()
Vertex_3 = model.selection("VERTEX", "pnt")

# Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(20, 10, 40, 10)
SketchLine_2 = Sketch_1.addLine(40, 10, 40, 20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(40, 20, 20, 20)
SketchLine_4 = Sketch_1.addLine(20, 20, 20, 10)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_2.addLine(20, 10, 40, 10)
SketchLine_6 = Sketch_2.addLine(40, 10, 40, 20)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_2.addLine(40, 20, 20, 20)
SketchLine_8 = Sketch_2.addLine(20, 20, 20, 10)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_5.startPoint(), SketchLine_8.endPoint())
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1s-SketchLine_4e")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_2/Vertex-SketchLine_7s-SketchLine_6e")])

# Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OZ"), 100, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection("EDGE", "PartSet/OX"), 100, 0)

# Parameters
model.addParameter(Part_1_doc, "dx", "10")
model.addParameter(Part_1_doc, "dy", "25")
model.addParameter(Part_1_doc, "dz", "15")

# Tests
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 0, 10, 10)
Box_3 = model.addBox(Part_1_doc, 10, 0, 10)
Box_4 = model.addBox(Part_1_doc, 10, 10, 0)
Box_5 = model.addBox(Part_1_doc, -10, 10, 10)
Box_6 = model.addBox(Part_1_doc, 10, -10, 10)
Box_7 = model.addBox(Part_1_doc, 10, 10, -10)
Box_8 = model.addBox(Part_1_doc, Point_1, Point_2)
Box_9 = model.addBox(Part_1_doc, Point_1, Point_1)
Box_10 = model.addBox(Part_1_doc, Point_1, Point_3)
Box_11 = model.addBox(Part_1_doc, Point_1, Point_4)
Box_12 = model.addBox(Part_1_doc, Point_1, Point_5)
Box_13 = model.addBox(Part_1_doc, Vertex_3, Point_2)
Box_14 = model.addBox(Part_1_doc, Point_2, Vertex_3)
Box_15 = model.addBox(Part_1_doc, model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1s-SketchLine_4e"), model.selection("VERTEX", "Sketch_2/Vertex-SketchLine_7s-SketchLine_6e"))
Box_16 = model.addBox(Part_1_doc, model.selection("VERTEX", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1"), model.selection("VERTEX", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/Generated_Face_1&Extrusion_2_1/To_Face_1"))
Box_17 = model.addBox(Part_1_doc, model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_2_1"))
Box_18 = model.addBox(Part_1_doc, "dx", "dy", "dz")
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Box_1, 1)
model.testNbSubResults(Box_1, [0])
model.testNbSubShapes(Box_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Box_1, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Box_1, model, Part_1_doc)

model.testNbResults(Box_8, 1)
model.testNbSubResults(Box_8, [0])
model.testNbSubShapes(Box_8, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Box_8, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Box_8, model, Part_1_doc)

model.testNbResults(Box_15, 1)
model.testNbSubResults(Box_15, [0])
model.testNbSubShapes(Box_15, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Box_15, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Box_15, model, Part_1_doc)

model.testNbResults(Box_16, 1)
model.testNbSubResults(Box_16, [0])
model.testNbSubShapes(Box_16, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Box_16, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Box_16, model, Part_1_doc)

model.testNbResults(Box_17, 1)
model.testNbSubResults(Box_17, [0])
model.testNbSubShapes(Box_17, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Box_17, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Box_17, model, Part_1_doc)

model.testNbResults(Box_18, 1)
model.testNbSubResults(Box_18, [0])
model.testNbSubShapes(Box_18, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Box_18, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Box_18, model, Part_1_doc)

model.testNbResults(Box_2, 0)
assert(Box_2.feature().error() == "Box builder with dimensions :: Dx is null or negative.")

model.testNbResults(Box_3, 0)
assert(Box_3.feature().error() == "Box builder with dimensions :: Dy is null or negative.")

model.testNbResults(Box_4, 0)
assert(Box_4.feature().error() == "Box builder with dimensions :: Dz is null or negative.")

model.testNbResults(Box_5, 0)
assert(Box_5.feature().error() == "Box builder with dimensions :: Dx is null or negative.")

model.testNbResults(Box_6, 0)
assert(Box_6.feature().error() == "Box builder with dimensions :: Dy is null or negative.")

model.testNbResults(Box_7, 0)
assert(Box_7.feature().error() == "Box builder with dimensions :: Dz is null or negative.")

model.testNbResults(Box_9, 0)
assert(Box_9.feature().error() == "Box builder with points :: the distance between the two points is null.")

model.testNbResults(Box_10, 0)
assert(Box_10.feature().error() == "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.")

model.testNbResults(Box_11, 0)
assert(Box_11.feature().error() == "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.")

model.testNbResults(Box_12, 0)
assert(Box_12.feature().error() == "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.")

model.testNbResults(Box_13, 0)
assert(Box_13.feature().error() == "Attribute \"FirstPoint\" is not initialized.")

model.testNbResults(Box_14, 0)
assert(Box_14.feature().error() == "Attribute \"SecondPoint\" is not initialized.")

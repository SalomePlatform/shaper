"""
Test case for Scale feature. 
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Create Boxes
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)
Box_4 = model.addBox(Part_1_doc, 10, 10, 10)
Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
Box_6 = model.addBox(Part_1_doc, 10, 10, 10)
Box_7 = model.addBox(Part_1_doc, 10, 10, 10)
Box_8 = model.addBox(Part_1_doc, 10, 10, 10)
Box_9 = model.addBox(Part_1_doc, 10, 10, 10)
Box_10 = model.addBox(Part_1_doc, 10, 10, 10)
Box_11 = model.addBox(Part_1_doc, 10, 10, 10)
Box_12 = model.addBox(Part_1_doc, 10, 10, 10)
Box_13 = model.addBox(Part_1_doc, 10, 10, 10)
Box_14 = model.addBox(Part_1_doc, 10, 10, 10)
Box_15 = model.addBox(Part_1_doc, 10, 10, 10)
Box_16 = model.addBox(Part_1_doc, 10, 10, 10)
Box_17 = model.addBox(Part_1_doc, 10, 10, 10)
Box_18 = model.addBox(Part_1_doc, 10, 10, 10)

# Create Sketches
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(11.50379459310504, -12.39258082761096, 58.75227313132206, 54.23988890577205)

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_2 = Sketch_2.addLine(41.58300156386008, 83.65088639460937, -69.84532559339473, 11.87498196448581)
SketchLine_3 = Sketch_2.addLine(-69.84532559339473, 11.87498196448581, -47.25850252097817, -30.78901717230093)
SketchLine_4 = Sketch_2.addLine(-47.25850252097817, -30.78901717230093, 108.3396119778911, -36.812169991612)
SketchLine_5 = Sketch_2.addLine(108.3396119778911, -36.812169991612, 35.05791934293975, 16.39234657896911)
SketchLine_6 = Sketch_2.addLine(35.05791934293975, 16.39234657896911, 41.58300156386008, 83.65088639460937)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_6.endPoint())
model.do()

# Create Points
Point_1 = model.addPoint(Part_1_doc, -10, -10, -10)

# Create Vertexes
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1e")])

# Create Wire & Extrusion
Wire_1 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_2/Wire-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 50, 0)

# Create Scales
Scale_1 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_1_1")] , model.selection("VERTEX", "PartSet/Origin"), 2)
Scale_2 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_2_1")] , model.selection("VERTEX", "Point_1"), 2)
Scale_3 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_3_1")] , model.selection("VERTEX", "Box_3_1/Back&Box_3_1/Left&Box_3_1/Top"), 2)
Scale_4 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_4_1")] , model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1e"), 2)
Scale_5 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_5_1")] , model.selection("VERTEX", "Vertex_1_1"), 2)
Scale_6 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_6_1")] , model.selection("VERTEX", "PartSet/Origin"), 0)
Scale_7 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_7_1")] , model.selection("VERTEX", "PartSet/Origin"), -3)
Scale_8 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_8_1")] , model.selection("VERTEX", "InvalidName"), 2)
Scale_9 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_9_1")] , model.selection("VERTEX", "PartSet/Origin"), 2 , 3, 1)
Scale_10 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_10_1")] , model.selection("VERTEX", "Point_1"), 2 , 3, 1)
Scale_11 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_11_1")] , model.selection("VERTEX", "Box_11_1/Back&Box_11_1/Right&Box_11_1/Top"), 2 , 3, 1)
Scale_12 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_12_1")] , model.selection("VERTEX", "Sketch_1/Vertex-SketchLine_1e"), 2 , 3, 1)
Scale_13 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_13_1")] , model.selection("VERTEX", "Vertex_1_1"), 2 , 3, 1)
Scale_14 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_14_1")] , model.selection("VERTEX", "PartSet/Origin"), 0 , 3, 1)
Scale_15 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_15_1")] , model.selection("VERTEX", "PartSet/Origin"), 2 , 0, 1)
Scale_16 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_16_1")] , model.selection("VERTEX", "PartSet/Origin"), 2 , 3, 0)
Scale_17 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_17_1")] , model.selection("VERTEX", "PartSet/Origin"), -2 , -3, -0.5)
Scale_18 = model.addScale(Part_1_doc, [model.selection("SOLID", "Box_18_1")] , model.selection("VERTEX", "InvalidName"), 2 , 3, 1)
Scale_19 = model.addScale(Part_1_doc, [model.selection("SHELL", "Extrusion_1_1")] , model.selection("VERTEX", "PartSet/Origin"), 5)

model.do()
model.end()


# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Scale_1, 1)
model.testNbSubResults(Scale_1, [0])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_1, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_1, model, Part_1_doc)

model.testNbResults(Scale_2, 1)
model.testNbSubResults(Scale_2, [0])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_2, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_2, model, Part_1_doc)

model.testNbResults(Scale_3, 1)
model.testNbSubResults(Scale_3, [0])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_3, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_3, model, Part_1_doc)

model.testNbResults(Scale_4, 1)
model.testNbSubResults(Scale_4, [0])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_4, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_4, model, Part_1_doc)

model.testNbResults(Scale_5, 1)
model.testNbSubResults(Scale_5, [0])
model.testNbSubShapes(Scale_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_5, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_5, model, Part_1_doc)

model.testNbResults(Scale_7, 1)
model.testNbSubResults(Scale_7, [0])
model.testNbSubShapes(Scale_7, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_7, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_7, model, Part_1_doc)

model.testNbResults(Scale_9, 1)
model.testNbSubResults(Scale_9, [0])
model.testNbSubShapes(Scale_9, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_9, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_9, model, Part_1_doc)

model.testNbResults(Scale_10, 1)
model.testNbSubResults(Scale_10, [0])
model.testNbSubShapes(Scale_10, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_10, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_10, model, Part_1_doc)

model.testNbResults(Scale_11, 1)
model.testNbSubResults(Scale_11, [0])
model.testNbSubShapes(Scale_11, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_11, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_11, model, Part_1_doc)

model.testNbResults(Scale_12, 1)
model.testNbSubResults(Scale_12, [0])
model.testNbSubShapes(Scale_12, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_12, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_12, model, Part_1_doc)

model.testNbResults(Scale_13, 1)
model.testNbSubResults(Scale_13, [0])
model.testNbSubShapes(Scale_13, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_13, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_13, model, Part_1_doc)

model.testNbResults(Scale_17, 1)
model.testNbSubResults(Scale_17, [0])
model.testNbSubShapes(Scale_17, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Scale_17, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Scale_17, model, Part_1_doc)


model.testNbResults(Scale_6, 0)
assert(Scale_6.feature().error() == 'Scale builder :: the scale factor is null.')

model.testNbResults(Scale_8, 0)
assert(Scale_8.feature().error() == 'Attribute "center_point" is not initialized.')

model.testNbResults(Scale_14, 0)
assert(Scale_14.feature().error() == 'Scale builder :: the scale factor in X is null.')

model.testNbResults(Scale_15, 0)
assert(Scale_15.feature().error() == 'Scale builder :: the scale factor in Y is null.')

model.testNbResults(Scale_16, 0)
assert(Scale_16.feature().error() == 'Scale builder :: the scale factor in Z is null.')

model.testNbResults(Scale_18, 0)
assert(Scale_18.feature().error() == 'Attribute "center_point" is not initialized.')

# To uncomment when #2046 will be performed
#model.testNbResults(Scale_19, 1)
#model.testNbSubResults(Scale_19, [0])
#model.testNbSubShapes(Scale_19, GeomAPI_Shape.SHELL, [1])
#model.testNbSubShapes(Scale_19, GeomAPI_Shape.SOLID, [0])
#model.testNbSubShapes(Scale_19, GeomAPI_Shape.FACE, [5])
#model.testHaveNamingFaces(Scale_19, model, Part_1_doc)
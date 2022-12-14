from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Create two points
Point_1 = model.addPoint(Part_1_doc, 0, 0, 0)
Point_2 = model.addPoint(Part_1_doc, 1, 0, 0)
model.do()

# Create a partition between two vertices with default tolerance:
# => partition results in a compound with 2 vertices
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_2")])
Partition_1_objects = [model.selection("VERTEX", "Vertex_1_1"), model.selection("VERTEX", "Vertex_2_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
model.do()

# Create a boolean fusion between two vertices with default tolerance:
# => fusion results in a compound with 2 vertices
Vertex_3 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Vertex_4 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_2")])
Fuse_1_objects = [model.selection("VERTEX", "Vertex_3_1"), model.selection("VERTEX", "Vertex_4_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects, True, keepSubResults = True)
model.do()

# Create a partition between two vertices with higher tolerance:
# => partition results in a single vertex
Vertex_5 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Vertex_6 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_2")])
LimitTolerance_1 = model.addLimitTolerance(Part_1_doc, model.selection("VERTEX", "Vertex_5_1"), 0.6)
LimitTolerance_2 = model.addLimitTolerance(Part_1_doc, model.selection("VERTEX", "Vertex_6_1"), 0.6)
Partition_2_objects = [model.selection("VERTEX", "LimitTolerance_1_1"), model.selection("VERTEX", "LimitTolerance_2_1")]
Partition_2 = model.addPartition(Part_1_doc, Partition_2_objects, keepSubResults = True)
model.do()

# Create a boolean fusion between two vertices with higher tolerance:
# => fusion results in a single vertex
Vertex_7 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Vertex_8 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_2")])
LimitTolerance_3 = model.addLimitTolerance(Part_1_doc, model.selection("VERTEX", "Vertex_7_1"), 0.6)
LimitTolerance_4 = model.addLimitTolerance(Part_1_doc, model.selection("VERTEX", "Vertex_8_1"), 0.6)
Fuse_2_objects = [model.selection("VERTEX", "LimitTolerance_3_1"), model.selection("VERTEX", "LimitTolerance_4_1")]
Fuse_2 = model.addFuse(Part_1_doc, Fuse_2_objects, True, keepSubResults = True)
model.end()

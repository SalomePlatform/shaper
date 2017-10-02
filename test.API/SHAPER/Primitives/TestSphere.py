from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Init
Point_1 = model.addPoint(Part_1_doc, 10, 10, 10).result()
Point_2 = model.selection("VERTEX", "pnt")

# Parameters
model.addParameter(Part_1_doc, "r", "5")

# Tests
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
Sphere_2 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 0)
Sphere_3 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), -5)
Sphere_4 = model.addSphere(Part_1_doc, Point_1, 10)
Sphere_5 = model.addSphere(Part_1_doc, Point_2, 10)
Sphere_6 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), "r")

model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Sphere_1, 1)
model.testNbSubResults(Sphere_1, [0])
model.testNbSubShapes(Sphere_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_1, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_1, model, Part_1_doc)

model.testNbResults(Sphere_4, 1)
model.testNbSubResults(Sphere_4, [0])
model.testNbSubShapes(Sphere_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_4, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_4, model, Part_1_doc)

model.testNbResults(Sphere_6, 1)
model.testNbSubResults(Sphere_6, [0])
model.testNbSubShapes(Sphere_6, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Sphere_6, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Sphere_4, model, Part_1_doc)

model.testNbResults(Sphere_2, 0)
assert(Sphere_2.feature().error() == "Sphere builder :: radius is negative or null.")

model.testNbResults(Sphere_3, 0)
assert(Sphere_3.feature().error() == "Sphere builder :: radius is negative or null.")

model.testNbResults(Sphere_5, 0)
assert(Sphere_5.feature().error() == "Attribute \"center_point\" is not initialized.")

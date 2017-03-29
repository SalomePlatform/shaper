"""
Test case for Translation feature. 
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Boxes
Box_1 = model.addBox(Part_1_doc, 10, 20, 10)
Box_2 = model.addBox(Part_1_doc, 10, 20, 10)
Box_3 = model.addBox(Part_1_doc, 10, 20, 10)
Box_4 = model.addBox(Part_1_doc, 10, 20, 10)
Box_5 = model.addBox(Part_1_doc, 10, 20, 10)
Box_6 = model.addBox(Part_1_doc, 10, 20, 10)
Box_7 = model.addBox(Part_1_doc, 10, 20, 10)
Box_8 = model.addBox(Part_1_doc, 10, 20, 10)

# Parameters
model.addParameter(Part_1_doc, "dx", "10")
model.addParameter(Part_1_doc, "dy", "15")
model.addParameter(Part_1_doc, "dz", "-5")

# Translations
Translation_91 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], 10, 20, 15)
Translation_92 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], 0, 20, 15)
Translation_93 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], 10, 0, 15)
Translation_94 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_4_1")], 10, 20, 0)
Translation_95 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], -10, 20, 15)
Translation_96 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_6_1")], 10, -20, 15)
Translation_97 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_7_1")], 10, 20, -15)
Translation_98 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_8_1")], "dx", "dy", "dz")
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Translation_91, 1)
model.testNbSubResults(Translation_91, [0])
model.testNbSubShapes(Translation_91, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_91, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_91, model, Part_1_doc)

model.testNbResults(Translation_92, 1)
model.testNbSubResults(Translation_92, [0])
model.testNbSubShapes(Translation_92, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_92, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_92, model, Part_1_doc)

model.testNbResults(Translation_93, 1)
model.testNbSubResults(Translation_93, [0])
model.testNbSubShapes(Translation_93, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_93, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_93, model, Part_1_doc)

model.testNbResults(Translation_94, 1)
model.testNbSubResults(Translation_94, [0])
model.testNbSubShapes(Translation_94, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_94, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_94, model, Part_1_doc)

model.testNbResults(Translation_95, 1)
model.testNbSubResults(Translation_95, [0])
model.testNbSubShapes(Translation_95, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_95, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_95, model, Part_1_doc)

model.testNbResults(Translation_96, 1)
model.testNbSubResults(Translation_96, [0])
model.testNbSubShapes(Translation_96, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_96, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_96, model, Part_1_doc)

model.testNbResults(Translation_97, 1)
model.testNbSubResults(Translation_97, [0])
model.testNbSubShapes(Translation_97, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_97, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_97, model, Part_1_doc)

model.testNbResults(Translation_98, 1)
model.testNbSubResults(Translation_98, [0])
model.testNbSubShapes(Translation_98, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_98, GeomAPI_Shape.FACE, [6])
model.testHaveNamingFaces(Translation_98, model, Part_1_doc)
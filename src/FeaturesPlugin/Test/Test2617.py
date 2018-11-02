# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cone_1_1")], model.selection("EDGE", "PartSet/OX"), 5)
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OZ"), True)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Symmetry_1_1_2"), model.selection("SOLID", "Symmetry_1_1_1")])
model.do()
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Common_1, [461.055845803990962394891539589])

assert(model.checkPythonDump())

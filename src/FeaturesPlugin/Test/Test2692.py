# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OY"), 20, 3)
ExtrusionCut_1_objects_2 = [model.selection("SOLID", "LinearCopy_1_1_2"), model.selection("SOLID", "LinearCopy_1_1_1"), model.selection("SOLID", "LinearCopy_1_1_3")]
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 10, ExtrusionCut_1_objects_2)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "LinearCopy_1_1_2/MF:Translated_Face&Box_1_1/Top"))
SketchCircle_1 = Sketch_1.addCircle(4.871972805908816, 24.69028692663239, 2.754280056405151)
SketchCircle_2 = Sketch_1.addCircle(4.956613458332676, 4.459837610261877, 1.82794741094275)
SketchCircle_3 = Sketch_1.addCircle(4.842959508111481, 44.32940059836771, 3.136980464700132)
ExtrusionCut_1.setNestedSketch(Sketch_1)
model.do()
model.testHaveNamingSubshapes(ExtrusionCut_1, model, Part_1_doc)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [3])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [21])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [90])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [180])
model.testResultsVolumes(ExtrusionCut_1, [2347.550996571865198347950354218])

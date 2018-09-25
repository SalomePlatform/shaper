# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(12, 12, 6.5)
SketchPoint_1 = Sketch_1.addPoint(13, 50)
SketchPoint_2 = Sketch_1.addPoint(30, 70)

SketchMultiTranslation_1 = Sketch_1.addTranslation([SketchCircle_1.results()[1]], SketchPoint_1.coordinates(), SketchPoint_2.coordinates(), 4)
[SketchCircle_2, SketchCircle_3, SketchCircle_4] = SketchMultiTranslation_1.translated()

model.do()
model.end()
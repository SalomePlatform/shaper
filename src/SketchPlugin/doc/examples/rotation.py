# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(45, 45)
SketchCircle_1 = Sketch_1.addCircle(35, 25, 8)

SketchMultiRotation_1 = Sketch_1.addRotation([SketchCircle_1.results()[1]], SketchPoint_1.coordinates(), 270, 4, True)
[SketchCircle_2, SketchCircle_3, SketchCircle_4] = SketchMultiRotation_1.rotated()

model.do()
model.end()

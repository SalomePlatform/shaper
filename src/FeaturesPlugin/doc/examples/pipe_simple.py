# -*- coding: utf-8 -*-

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 40)
model.do()
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchArc_1 = Sketch_2.addArc(30, 0, 0, 0, 40.50885209855912, 28.09918197333542, True)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchAPI_Line(SketchLine_1).startPoint(), SketchArc_1.startPoint())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchArc_1.results()[1], 30)
model.do()
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "Sketch_2/SketchArc_1_2"))
model.do()
model.end()

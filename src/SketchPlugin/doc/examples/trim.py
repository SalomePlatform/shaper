# -*- coding: utf-8 -*-

from salome.shaper import model
from salome.shaper import geom

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

SketchCircle_1 = Sketch_1.addCircle(40, 45, 30)

SketchPoint_1 = Sketch_1.addPoint(20, 70)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.coordinates(), SketchCircle_1.results()[1])

SketchLine_1 = Sketch_1.addLine(17, 7, 80, 70)

GeomPoint = geom.Pnt2d(47, 75)
Sketch_1.addTrim(SketchCircle_1, GeomPoint)

model.do()
model.end()

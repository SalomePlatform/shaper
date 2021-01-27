from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(65.56914119359536, 34.84279475982533, -62.33478893740904, 34.84279475982533)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(-62.33478893740904, 34.84279475982533, -62.33478893740904, -28.08005822416302)

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(-62.33478893740904, -28.08005822416302, 65.56914119359536, -28.08005822416302)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(65.56914119359536, -28.08005822416302, 65.56914119359536, 34.84279475982533)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_3.result())
Sketch_1.setVertical(SketchLine_4.result())
model.do()

### Create Plane
Plane_1 = model.addPlane(Part_1_doc, model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("VERTEX", "Sketch_1/SketchLine_3_StartVertex"), False)

### Create Plane
Plane_2 = model.addPlane(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f"), 100, False)

### Create Loft
Loft_1 = model.addLoft(Part_1_doc, model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2"))

model.end()

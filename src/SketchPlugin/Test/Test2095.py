from SketchAPI import *
from salome.shaper import model

lineStart = [26, 53]
lineEnd = [71, 30]

model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(lineStart[0], lineStart[1], lineEnd[0], lineEnd[1])
SketchLine_2 = Sketch_1.addLine(model.selection("EDGE", "OX"))
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_2.result(), [SketchLine_1.result()])
[SketchLine_3] = SketchConstraintMirror_1.mirrored()

SketchLine_4 = Sketch_1.addLine(lineEnd[0], lineEnd[1], lineEnd[0], -lineEnd[1])
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchAPI_Line(SketchLine_3).endPoint(), SketchLine_4.endPoint())

Sketch_1.setFillet(SketchLine_1.endPoint())
model.do()

assert(SketchLine_1.startPoint().x() ==  SketchAPI_Line(SketchLine_3).startPoint().x())
assert(SketchLine_1.startPoint().y() == -SketchAPI_Line(SketchLine_3).startPoint().y())
assert(SketchLine_1.endPoint().x() ==  SketchAPI_Line(SketchLine_3).endPoint().x())
assert(SketchLine_1.endPoint().y() == -SketchAPI_Line(SketchLine_3).endPoint().y())
assert(SketchLine_1.endPoint().x() != lineEnd[0])
assert(SketchLine_1.endPoint().y() != lineEnd[1])

model.end()

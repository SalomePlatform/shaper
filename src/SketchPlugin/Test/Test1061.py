"""
    Test1061.py
    Test case for issue #1061 "Distance constraint using for points with equal coordinates"
"""

import math
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(70, 120, 50, 25)
SketchLine_2 = Sketch_1.addLine(50, 25, 170, 50)

DISTANCE = 100
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.endPoint(), SketchLine_2.startPoint(), DISTANCE)
model.do()

# check distance between points
aDist2 = (SketchLine_1.endPoint().x() - SketchLine_2.startPoint().x())**2 + (SketchLine_1.endPoint().y() - SketchLine_2.startPoint().y())**2
assert(math.fabs(aDist2 - DISTANCE**2) > 1e-12)

model.end()

assert(model.checkPythonDump())

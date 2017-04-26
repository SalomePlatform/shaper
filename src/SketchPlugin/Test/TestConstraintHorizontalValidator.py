"""
    TestConstraintHorizontalValidator.py
    It tests validation of horizontal and vertical segments in H and V constraints to avoid
    selection segments that already have one of these constraint"
"""

#=========================================================================
# of the test
#=========================================================================
from salome.shaper import model
from ModelAPI import *
import math
#=========================================================================
# Creation of a part
#=========================================================================
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
#=========================================================================
# Creation of a sketch
#=========================================================================
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(20, 20, 40, 80)
SketchLine_1.setName("SketchLine_1")
SketchLine_1.result().setName("SketchLine_1")
SketchLine_2 = Sketch_1.addLine(40, 80, 60, 40)
SketchLine_2.setName("SketchLine_2")
SketchLine_2.result().setName("SketchLine_2")
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintHorizontal_1.setName("SketchConstraintHorizontal_1")
SketchConstraintVertical_1.setName("SketchConstraintVertical_1")
model.do()
#=========================================================================
# Checking that sketch and constraints become invalid when to one line
# two horizontal/vertical constraints are applied in any combination
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Sketch_1.feature()))
assert(Sketch_1.feature().error() == "Sketch objects are not defined")
#=========================================================================
# Remove duplicated Vertical constraints
#=========================================================================
Part_1_doc.removeFeature(SketchConstraintVertical_1.feature())
assert(aFactory.validate(Sketch_1.feature()))
model.do()
#=========================================================================
# Checking that after excess constraints are removed or undone,
# sketch becomes valid.
#=========================================================================
assert(aFactory.validate(Sketch_1.feature()))
assert(Sketch_1.feature().error() == '')

#assert(model.checkPythonDump())

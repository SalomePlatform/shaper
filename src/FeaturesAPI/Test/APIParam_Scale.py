"""
Test case for Scale feature. 
Written on High API.
"""
from ModelAPI import *
from GeomAPI import *

from salome.shaper import model

# Get session
aSession = ModelAPI_Session.get()

# Create a part
aDocument = aSession.activeDocument()
aSession.startOperation()
model.addPart(aDocument)
aDocument = aSession.activeDocument()
aSession.finishOperation()

# Create a box
aSession.startOperation()
aCenterPoint = model.addPoint(aDocument, 20, 20, 0).result()
aBox1 =  model.addBox(aDocument, 10, 10, 10)
aSession.finishOperation()

# Perform a symmetry by a point
aSession.startOperation()
aScale1 = model.addScale(aDocument, [model.selection("SOLID", "Box_1_1")], aCenterPoint, 2.0).result()
aSession.finishOperation()
assert (aScale1 is not None)
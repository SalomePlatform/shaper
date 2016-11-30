"""
Test case for Primitive Box feature. 
Written on High API.
"""
from ModelAPI import *
from GeomAPI import *

import model

# Get session
aSession = ModelAPI_Session.get()

# Create a part
aDocument = aSession.activeDocument()
aSession.startOperation()
model.addPart(aDocument)
aDocument = aSession.activeDocument()
aSession.finishOperation()

# Create a box with dimensions
aSession.startOperation()
aBox1 = model.addBox(aDocument, 10, 10, 10).result()
aSession.finishOperation()
assert (aBox1 is not None)

# Create a first point
aSession.startOperation()
aFirstPoint = model.addPoint(aDocument, 0, 0, 0).result()
aSession.finishOperation()

# Create a second point
aSession.startOperation()
aSecondPoint = model.addPoint(aDocument, 50, 50, 50).result()
aSession.finishOperation()

# Create a box with 2 points
aSession.startOperation()
aBox2 = model.addBox(aDocument, aFirstPoint, aSecondPoint).result()
aSession.finishOperation()
assert (aBox2 is not None)

# Create a box with dimensions (error)
aSession.startOperation()
aBox3 = model.addBox(aDocument, -10, 10, 10).result()
aSession.finishOperation()
assert (aBox3 is not None)

# Create a box with 2 points (error)
aSession.startOperation()
aBox4 = model.addBox(aDocument, aFirstPoint, aFirstPoint).result()
aSession.finishOperation()
assert (aBox4 is not None)


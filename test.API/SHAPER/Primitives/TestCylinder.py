"""
Test case for Primitive Cylinder feature. 
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

aSession.startOperation()
aBasePoint = model.addPoint(aDocument, 0, 0, 0).result()
anAxis = model.addAxis(aDocument, 10, 0, 0).result()
aSession.finishOperation()

aSession.startOperation()
aCylinder1 = model.addCylinder(aDocument, aBasePoint, anAxis, 5., 20.)
assert (aCylinder1 is not None)
aSession.finishOperation()

aSession.startOperation()
aCylinder2 = model.addCylinder(aDocument, aBasePoint, anAxis, 5., 20., 100.)
assert (aCylinder2 is not None)
aSession.finishOperation()

aSession.startOperation()
aCylinder3 = model.addCylinder(aDocument, 5., 20.)
assert (aCylinder3 is not None)
aSession.finishOperation()

aSession.startOperation()
aCylinder4 = model.addCylinder(aDocument, 5., 20., 100.)
assert (aCylinder4 is not None)
aSession.finishOperation()


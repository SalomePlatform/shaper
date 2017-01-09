"""
Test case for Translation feature. 
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
aBox1 =  model.addBox(aDocument, 10, 10, 10)
aBox2 =  model.addBox(aDocument, 10, 10, 10)
aBox3 =  model.addBox(aDocument, 10, 10, 10)

# Perform a symmetry by a point
aSession.startOperation()
aPoint = model.addPoint(aDocument, 0, 0, 0).result()
aSymmetry1 = model.addSymmetry(aDocument, [model.selection("SOLID", "Box_1_1")], aPoint).result()
aSession.finishOperation()
assert (aSymmetry1 is not None)

# Perform a symmetry by an axis
aSession.startOperation()
anAxis = model.addAxis(aDocument, 10, 0, 0).result()
aSymmetry2 = model.addSymmetry(aDocument, [model.selection("SOLID", "Box_2_1")], anAxis).result()
aSession.finishOperation()
assert (aSymmetry2 is not None)

# Perform a symmetry by a plane
aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0).result()
aPoint2 = model.addPoint(aDocument, 10, 10, 0).result()
aSymmetry3 = model.addSymmetry(aDocument, [model.selection("SOLID", "Box_3_1")], model.selection("FACE", "Box_3_1/Top")).result()
aSession.finishOperation()
assert (aSymmetry3 is not None)
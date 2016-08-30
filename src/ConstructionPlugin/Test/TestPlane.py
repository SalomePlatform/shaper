"""
Test case for Construction Plane feature. Written on High API.
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

# Test a plane by general equation
aSession.startOperation()
aPlane = model.addPlane(aDocument, 1, 1, 1, 0)
aSession.finishOperation()
assert (len(aPlane.result()) > 0)

# Create a point 1
aSession.startOperation()
aPoint1 = model.addPoint(aDocument, 0, 0, 0)
aSession.finishOperation()

# Create a point 2
aSession.startOperation()
aPoint2 = model.addPoint(aDocument, 100, 0, 0)
aSession.finishOperation()

# Create a point 3
aSession.startOperation()
aPoint3 = model.addPoint(aDocument, 50, 50, 50)
aSession.finishOperation()

# Test a plane by three points
aSession.startOperation()
aPlane = model.addPlane(aDocument, aPoint1.result()[0], aPoint2.result()[0], aPoint3.result()[0])
aSession.finishOperation()
assert (len(aPlane.result()) > 0)

# Create an axis
aSession.startOperation()
anAxis = model.addAxis(aDocument, 0, 0, 100)
aSession.finishOperation()

# Test a plane by line and point
aSession.startOperation()
aPlane = model.addPlane(aDocument, anAxis.result()[0], aPoint3.result()[0], True)
aSession.finishOperation()
assert (len(aPlane.result()) > 0)

# Test a plane by distance from other
aSession.startOperation()
aPlane = model.addPlane(aDocument, aPlane.result()[0], 50, False)
aSession.finishOperation()
assert (len(aPlane.result()) > 0)

# Test a plane by coincidence to point
aSession.startOperation()
aCoincidentPlane = model.addPlane(aDocument, aPlane.result()[0], aPoint2.result()[0])
aSession.finishOperation()
assert (len(aCoincidentPlane.result()) > 0)

# Create an axis
aSession.startOperation()
anAxis = model.addAxis(aDocument, 100, 0, 0)
aSession.finishOperation()

# Test a plane by rotation
aSession.startOperation()
aRotatedPlane = model.addPlane(aDocument, aCoincidentPlane.result()[0], anAxis.result()[0], 45)
aSession.finishOperation()
assert (len(aRotatedPlane.result()) > 0)

# Test plane by two parallel planes
aSession.startOperation()
aPlane = model.addPlane(aDocument, aCoincidentPlane.result()[0], aPlane.result()[0])
aSession.finishOperation()
assert (len(aPlane.result()) > 0)

import model
assert(model.checkPythonDump())
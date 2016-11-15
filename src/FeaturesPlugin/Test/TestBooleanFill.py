"""
Test case for Boolean Fill feature. Written on High API.
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

# Create a sketch with circle to extrude
aSession.startOperation()
anOrigin = GeomAPI_Pnt(0, 0, 0)
aDirX = GeomAPI_Dir(1, 0, 0)
aNorm = GeomAPI_Dir(0, 0, 1)
aCircleSketch = model.addSketch(aDocument, GeomAPI_Ax3(anOrigin, aDirX, aNorm))
aCircleSketch.addCircle(0, 0, 50)
aSession.finishOperation()

# Create a sketch with triangle to extrude
aSession.startOperation()
aTriangleSketch = model.addSketch(aDocument, GeomAPI_Ax3(anOrigin, aDirX, aNorm))
aTriangleSketch.addLine(25, 25, 100, 25)
aTriangleSketch.addLine(100, 25, 60, 75)
aTriangleSketch.addLine(60, 75, 25, 25)
aSession.finishOperation()

# Make extrusion on circle (cylinder) and triangle (prism)
aSession.startOperation()
anExtrusion = model.addExtrusion(aDocument, aCircleSketch.results() + aTriangleSketch.results(), 100)
aSession.finishOperation()

# Fill prism with cylinder
aSession.startOperation()
aBoolean = model.addFill(aDocument, [anExtrusion.results()[0]], [anExtrusion.results()[1]])
assert (len(aBoolean.results()) > 0)
aSession.finishOperation()

assert(model.checkPythonDump())

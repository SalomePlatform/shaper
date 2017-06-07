## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

"""
Test case for Boolean Fill feature. Written on High API.
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

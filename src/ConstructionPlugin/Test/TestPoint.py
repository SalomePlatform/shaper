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
Test case for Construction Point feature. Written on High API.
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

# Create a point by coordinates
aSession.startOperation()
aPoint = model.addPoint(aDocument, 50, 50, 50)
aSession.finishOperation()
assert (len(aPoint.results()) > 0)

# # Create a sketch with lines
# aSession.startOperation()
# anOrigin = GeomAPI_Pnt(0, 0, 0)
# aDirX = GeomAPI_Dir(1, 0, 0)
# aNorm = GeomAPI_Dir(0, 0, 1)
# aSketch = model.addSketch(aDocument, GeomAPI_Ax3(anOrigin, aDirX, aNorm))
# aSketchLine1 = aSketch.addLine(0, 0, 100, 100)
# aSketchLine2 = aSketch.addLine(0, 100, 100, 0)
# aSession.finishOperation()
#
# # Create a point on line
# aSession.startOperation()
# aPoint = model.addPoint(aDocument, aSketchLine1.result()[0], 25, True, False)
# aSession.finishOperation()
# assert (len(aPoint.result()) > 0)
#
# # Create plane
# aSession.startOperation()
# aPlane = model.addPlane(aDocument, 1, 1, 1, 1)
# aSession.finishOperation()
#
# # Create a point by projection
# aSession.startOperation()
# aPoint = model.addPoint(aDocument, aPoint.result()[0], aPlane.result()[0])
# aSession.finishOperation()
# assert (len(aPoint.result()) > 0)
#
# # Create a point by lines intersection
# aSession.startOperation()
# aPoint = model.addPoint(aDocument, aSketchLine1.result()[0], aSketchLine2.result()[0])
# aSession.finishOperation()
# assert (len(aPoint.result()) > 0)
#
# # Create a point by line and plane intersection
# aSession.startOperation()
# aPoint = model.addPoint(aDocument, aSketchLine1.result()[0], aPlane.result()[0])
# aSession.finishOperation()
# assert (len(aPoint.result()) > 0)

assert(model.checkPythonDump())

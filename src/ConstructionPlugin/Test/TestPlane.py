# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
Test case for Construction Plane feature. Written on High API.
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

# Test a plane by general equation
aSession.startOperation()
aPlane = model.addPlane(aDocument, 1, 1, 1, 0)
aSession.finishOperation()
assert (len(aPlane.results()) > 0)

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
aPlane = model.addPlane(aDocument, aPoint1.result(), aPoint2.result(), aPoint3.result())
aSession.finishOperation()
assert (len(aPlane.results()) > 0)

# Create an axis
aSession.startOperation()
anAxis = model.addAxis(aDocument, 0, 0, 100)
aSession.finishOperation()

# Test a plane by line and point
aSession.startOperation()
aPlane = model.addPlane(aDocument, anAxis.result(), aPoint3.result(), True)
aSession.finishOperation()
assert (len(aPlane.results()) > 0)

# Test a plane by distance from other
aSession.startOperation()
aPlane = model.addPlane(aDocument, aPlane.result(), 50, False)
aSession.finishOperation()
assert (len(aPlane.results()) > 0)

# Test a plane by coincidence to point
aSession.startOperation()
aCoincidentPlane = model.addPlane(aDocument, aPlane.result(), aPoint2.result())
aSession.finishOperation()
assert (len(aCoincidentPlane.results()) > 0)

# Create an axis
aSession.startOperation()
anAxis = model.addAxis(aDocument, 100, 0, 0)
aSession.finishOperation()

# Test a plane by rotation
aSession.startOperation()
aRotatedPlane = model.addPlane(aDocument, aCoincidentPlane.result(), anAxis.result(), 45)
aSession.finishOperation()
assert (len(aRotatedPlane.results()) > 0)

# Test plane by two parallel planes
aSession.startOperation()
aPlane = model.addPlane(aDocument, aCoincidentPlane.result(), aPlane.result())
aSession.finishOperation()
assert (len(aPlane.results()) > 0)

assert(model.checkPythonDump())

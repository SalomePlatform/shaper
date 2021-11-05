# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

# Create an axis
aSession.startOperation()
anAxis = model.addAxis(aDocument, 100, 0, 0)
aSession.finishOperation()

# Test a plane by rotation
aSession.startOperation()
aRotatedPlane = model.addPlane(aDocument, aPlane.result(), anAxis.result(), 45)
aRotatedPlanes = model.addPlane(aDocument, aPlane.result(), anAxis.result(), 45, 5)
aSession.finishOperation()
assert (len(aRotatedPlane.results()) > 0)
assert (len(aRotatedPlanes.results()) == 5)

# Test a plane by distance from other
aSession.startOperation()
anOnlyPlane = model.addPlane(aDocument, aPlane.result(), 50, False)
assert (len(anOnlyPlane.results()) > 0)
aPlane = model.addPlane(aDocument, aPlane.result(), 50, False, 10)
aSession.finishOperation()
assert (len(aPlane.results()) == 10)

assert(model.checkPythonDump())

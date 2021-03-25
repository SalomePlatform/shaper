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
      TestResults.py
      Cover specific working with the data model results

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *

__updated__ = "2016-20-16"

aSession = ModelAPI_Session.get()

#=========================================================================
# Check getting the initial construction plane by name
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aPlaneX = aPartSet.addFeature("Plane")
aPlaneX.string("creation_method").setValue("by_general_equation")
aPlaneX.real("A").setValue(1.)
aPlaneX.real("B").setValue(0.)
aPlaneX.real("C").setValue(0.)
aPlaneX.real("D").setValue(0.)
assert(aPlaneX)
aSession.finishOperation()

aSession.startOperation()
aColors = aPlaneX.lastResult().data().intArray("Color")
assert(aColors)
# default colors, not filled array
assert(aColors.size() == 0)
aColors.setSize(3)
aColors.setValue(0, 100)
aColors.setValue(1, 200)
aColors.setValue(2, 250)
assert(aColors.size() == 3)
assert(aColors.value(0) == 100)
assert(aColors.value(1) == 200)
assert(aColors.value(2) == 250)
aSession.finishOperation()

from salome.shaper import model
assert(model.checkPythonDump())

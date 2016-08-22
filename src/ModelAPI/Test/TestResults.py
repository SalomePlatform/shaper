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

import model
assert(model.checkPythonDump())

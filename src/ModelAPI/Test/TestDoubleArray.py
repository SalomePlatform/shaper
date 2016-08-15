#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

import math

aSession = ModelAPI_Session.get()

#=========================================================================
# Test double array attribute
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPartSet.addFeature("Sketch"))
aFeatureData = aSketchFeature.data()
anArray = aFeatureData.addAttribute("double_array", "DoubleArray")
aFeatureData.realArray("double_array").setSize(5)
assert(aFeatureData.realArray("double_array").size() == 5)
aFeatureData.realArray("double_array").setValue(0, 1)
assert(math.fabs(aFeatureData.realArray("double_array").value(0) - 1) < 10 ** -7)
aFeatureData.realArray("double_array").setValue(1, 1.5)
assert(math.fabs(aFeatureData.realArray("double_array").value(1) - 1.5) < 10 ** -7)

#=========================================================================
# End of test
#=========================================================================

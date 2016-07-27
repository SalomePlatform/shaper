"""
      Test1064.py
      Unit test for testing the Part sub-shapes naming, described in the issue 1064

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2015-10-16"

aSession = ModelAPI_Session.get()

#=========================================================================
# Create a sketch triangle in PartSet
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPartSet.addFeature("Sketch"))
aFeatureData = aSketchFeature.data()
anArray = aFeatureData.addAttribute("IntArray_1", "IntArray")
aFeatureData.intArray("IntArray_1").setSize(5)
assert(aFeatureData.intArray("IntArray_1").size() == 5)

#=========================================================================
# End of test
#=========================================================================
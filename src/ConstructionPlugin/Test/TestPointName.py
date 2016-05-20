from ModelAPI import *

aSession = ModelAPI_Session.get()
aDoc = aSession.moduleDocument()

aSession.startOperation()
aFeature = aDoc.addFeature("Point")
aFeatureData = aFeature.data()
assert(aFeatureData is not None)
aFeatureData.real("x").setValue(0.)
aFeatureData.real("y").setValue(0.)
aFeatureData.real("z").setValue(0.)
aFeatureName = aFeature.name()
aFeature.execute()
aSession.finishOperation()

# with index "0" is Origin and then - all default constructions
aFeature1 = aDoc.object("Construction", 7)
aFeature1Name = aFeature1.data().name()

assert (aFeatureName == aFeature1Name)

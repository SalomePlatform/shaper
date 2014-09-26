from ModelAPI import *

aSession = ModelAPI_Session.get()
aDoc = aSession.moduleDocument()

aSession.startOperation()
aFeature = aDoc.addFeature("Point")
aFeatureData = aFeature.data()
aFeatureData.real("x").setValue(0.)
aFeatureData.real("y").setValue(0.)
aFeatureData.real("z").setValue(0.)
aFeatureName = aFeatureData.name()
aFeature.execute()
aSession.finishOperation()

aFeature1 = aDoc.object("Construction", 0)
aFeature1Name = aFeature1.data().name()

assert (aFeatureName == aFeature1Name)

from ModelAPI import *
aSession = ModelAPI_Session.get()
aDoc = aSession.moduleDocument()
assert(not aSession.canUndo())
assert(not aSession.canRedo())

aSession.startOperation()
aFeature = aDoc.addFeature("Point")
aFeatureData = aFeature.data()
# Since validators are introduced we have to initialize all
# the feature's attributes
aFeatureData.real("x").setValue(1.)
aFeatureData.real("y").setValue(-1.)
aFeatureData.real("z").setValue(0.)
aFeatureName = aFeatureData.name()
assert(aFeatureName == "Point_1")

aFeature.execute()
aSession.finishOperation()

assert(aDoc.size("Construction") == 1)
assert(aSession.canUndo())
assert(not aSession.canRedo())

aSession.undo()
assert(aDoc.size("Construction") == 0)
assert(not aSession.canUndo())
assert(aSession.canRedo())

aSession.redo()
assert(aDoc.size("Construction") == 1)
assert(aSession.canUndo())
assert(not aSession.canRedo())

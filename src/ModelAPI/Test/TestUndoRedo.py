from ModelAPI import *
aSession = ModelAPI_Session.get()
aDoc = aSession.moduleDocument()
assert(not aSession.canUndo())
assert(not aSession.canRedo())

aSession.startOperation()
aFeature = aDoc.addFeature("Point")
# Since validators are introduced we have to initialize all
# the feature's attributes
# aFeature.string("creation_method").setValue("by_xyz")
aFeature.real("x").setValue(1.)
aFeature.real("y").setValue(-1.)
aFeature.real("z").setValue(0.)
aFeatureName = aFeature.name()
# "2" is because Origin is the first point
assert(aFeatureName == "Point_2")

aFeature.execute()
aSession.finishOperation()

assert(aDoc.size("Construction") == 8)
assert(aSession.canUndo())
assert(not aSession.canRedo())

aSession.undo()
assert(aDoc.size("Construction") == 7)
assert(not aSession.canUndo())
assert(aSession.canRedo())

aSession.redo()
assert(aDoc.size("Construction") == 8)
assert(aSession.canUndo())
assert(not aSession.canRedo())

import model
assert(model.checkPythonDump())

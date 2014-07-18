from ModelAPI import *
plugin_manager = ModelAPI_PluginManager.get()
doc = plugin_manager.rootDocument()
assert(not doc.canUndo())
assert(not doc.canRedo()) 

doc.startOperation()
feature = doc.addFeature("Point")
feature_name = feature.data().name()
assert(feature_name == "Point_1")

feature.execute()
doc.finishOperation();
assert(doc.size("Construction") == 1)
assert(doc.canUndo())
assert(not doc.canRedo()) 

doc.undo()
assert(doc.size("Construction") == 0)
assert(not doc.canUndo())
assert(doc.canRedo())

doc.redo()
assert(doc.size("Construction") == 1)
assert(doc.canUndo())
assert(not doc.canRedo())

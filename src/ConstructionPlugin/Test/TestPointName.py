from ModelAPI import *
p = ModelAPI_PluginManager.get()
f = p.rootDocument().addFeature("Point")
f_name = f.data().name()
f.execute()

doc = p.rootDocument()
f1 = doc.object("Construction", 0)
f1_name = f1.data().name()

assert (f_name == f1_name)

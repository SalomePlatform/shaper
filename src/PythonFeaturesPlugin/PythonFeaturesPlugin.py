import ModelAPI
from PythonFeaturesPlugin_Box import PythonFeaturesPlugin_Box

class PythonFeaturesPlugin(ModelAPI.ModelAPI_Plugin):
  
  def __init__(self):
    ModelAPI.ModelAPI_Plugin.__init__(self)
    pass

  def createFeature(self, theFeatureID):
    if theFeatureID == PythonFeaturesPlugin_Box.ID():
      return PythonFeaturesPlugin_Box()
    else:
      raise StandardError("No such feature %s"%theFeatureID)

plugin = PythonFeaturesPlugin()
aSession = ModelAPI.ModelAPI_Session.get()
print "Module loaded. Session", aSession
aSession.registerPlugin(plugin)

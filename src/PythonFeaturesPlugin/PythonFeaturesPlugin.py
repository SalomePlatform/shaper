from ModelAPI import *
import PythonFeaturesPlugin_Box

class PythonFeaturesPlugin(ModelAPI_Plugin):
  def __init__(self):
    ModelAPI_Plugin.__init__(self)
    pass

  def createFeature(self, theFeatureID):
    if theFeatureID == PythonFeaturesPlugin_Box.ID():
      return PythonFeaturesPlugin_Box()
    else:
      raise StandardError("No such feature %s"%theFeatureID)

plugin = PythonFeaturesPlugin()
ModelAPI_Session_get().registerPlugin(plugin)


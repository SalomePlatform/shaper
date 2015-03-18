"""
"""

import ModelAPI
from PythonFeaturesPlugin_Box import PythonFeaturesPlugin_Box


class PythonFeaturesPlugin(ModelAPI.ModelAPI_Plugin):

    def __init__(self):
        ModelAPI.ModelAPI_Plugin.__init__(self)
        aSession = ModelAPI.ModelAPI_Session.get()
        aSession.registerPlugin(self)
        pass

    def createFeature(self, theFeatureID):
        aFeature = None
        if theFeatureID == PythonFeaturesPlugin_Box.ID():
            aCompositeFeature = PythonFeaturesPlugin_Box().__disown__()
            aFeature = ModelAPI.compositeFeatureToFeature(aCompositeFeature)
        else:
            raise StandardError("No such feature %s" % theFeatureID)
        return aFeature


plugin = PythonFeaturesPlugin()
plugin.__disown__()

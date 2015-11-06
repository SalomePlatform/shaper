"""Registration of all user-defined Python features
"""

import ModelAPI
from macros.box.feature      import BoxFeature


class PythonFeaturesPlugin(ModelAPI.ModelAPI_Plugin):
    """Implementation of features plugin"""

    def __init__(self):
        """Constructor"""
        ModelAPI.ModelAPI_Plugin.__init__(self)
        aSession = ModelAPI.ModelAPI_Session.get()
        aSession.registerPlugin(self)
        pass

    def createFeature(self, theFeatureID):
        """Create a feature by its Id"""
        aFeature = None

        if theFeatureID == BoxFeature.ID():
            aFeature = BoxFeature().__disown__()

        else:
            raise StandardError("No such feature %s" % theFeatureID)

        return aFeature

### The plugin object
plugin = PythonFeaturesPlugin()
plugin.__disown__()

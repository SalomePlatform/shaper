"""Registration of all user-defined Python features
"""

import ModelAPI
from macros.box.feature      import BoxFeature


class PythonFeaturesPlugin(ModelAPI.ModelAPI_Plugin):
    """Class for Python features plugin.

    PythonFeaturesPlugin() -> plugin object
    """

    def __init__(self):
        """x.__init__(...) initializes x; see x.__class__.__doc__ for signature"""
        ModelAPI.ModelAPI_Plugin.__init__(self)
        aSession = ModelAPI.ModelAPI_Session.get()
        aSession.registerPlugin(self)
        pass

    def createFeature(self, theFeatureID):
        """Override ModelAPI_Plugin.createFeature()"""
        aFeature = None

        if theFeatureID == BoxFeature.ID():
            aFeature = BoxFeature().__disown__()

        else:
            raise StandardError("No such feature %s" % theFeatureID)

        return aFeature


plugin = PythonFeaturesPlugin()
plugin.__disown__()

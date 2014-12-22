"""
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import ModelAPI

from ConnectorPlugin_ExportFeature import ExportFeature


class ConnectorPlugin(ModelAPI.ModelAPI_Plugin):

    def __init__(self):
        ModelAPI.ModelAPI_Plugin.__init__(self)
        pass

    def createFeature(self, theFeatureID):
        if theFeatureID == ExportFeature.ID():
            return ExportFeature().__disown__()
        else:
            print "ConnectorPlugin: No such feature %s" % theFeatureID

plugin = ConnectorPlugin()
aSession = ModelAPI.ModelAPI_Session.get()
print "Module loaded. Session", aSession
aSession.registerPlugin(plugin)

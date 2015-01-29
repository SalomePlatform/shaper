## @package Plugins
#  Python plugin for exporting entities into Salome's GEOM module
#  Copyright (C) 2014-20xx CEA/DEN, EDF R&D

import ModelAPI

from ConnectorPlugin_ExportFeature import ExportFeature

## @ingroup Plugins
#  The main class for management the construction features as plugin.
class ConnectorPlugin(ModelAPI.ModelAPI_Plugin):

    ## The constructor.
    def __init__(self):
        ModelAPI.ModelAPI_Plugin.__init__(self)
        pass

    ## Creates the feature objects of this plugin by the feature string ID.
    def createFeature(self, theFeatureID):
        if theFeatureID == ExportFeature.ID():
            return ExportFeature().__disown__()
        else:
            print "ConnectorPlugin: No such feature %s" % theFeatureID

## The plugin created on module importing (from c++)
plugin = ConnectorPlugin()
## Main session of the application
aSession = ModelAPI.ModelAPI_Session.get()
aSession.registerPlugin(plugin)

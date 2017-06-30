## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

## @package Plugins
#  Python plugin for exporting entities into Salome's GEOM module

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

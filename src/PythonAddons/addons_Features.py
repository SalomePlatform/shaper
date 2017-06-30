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

"""Registration of all user-defined Python features
"""

import ModelAPI
from macros.box.feature       import BoxFeature
from macros.rectangle.feature import SketchPlugin_Rectangle


class PythonFeaturesPlugin(ModelAPI.ModelAPI_Plugin):
    """Implementation of features plugin.

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

        elif theFeatureID == SketchPlugin_Rectangle.ID():
            aFeature = SketchPlugin_Rectangle().__disown__()

        else:
            raise StandardError("No such feature %s" % theFeatureID)

        return aFeature

# The plugin object
plugin = PythonFeaturesPlugin()
plugin.__disown__()

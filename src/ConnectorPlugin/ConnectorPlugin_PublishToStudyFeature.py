# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

## @package Plugins
#  ExportFeature class definition

import ModelAPI
import ExchangeAPI
import EventsAPI

import salome
from salome.shaper import model

import os
import salome


## @ingroup Plugins
#  Feature to export all shapes and groups into the GEOM module
class PublishToStudyFeature(ModelAPI.ModelAPI_Feature):

    ## The constructor.
    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)
        pass

    @staticmethod
    ## Export kind. Static.
    def ID():
        return "PublishToStudy"

    ## Returns the kind of a feature.
    def getKind(self):
        return PublishToStudyFeature.ID()

    ## This feature is action: has no property panel and executes immediately.
    def isAction(self):
        return True

    ## This feature has no attributes, as it is action.
    def initAttributes(self):
        pass

    ## Exports all shapes and groups into the GEOM module.
    def execute(self):
        print("### Execution of PublishToStudy")

        # find a shaper-study component
        salome.salome_init(1)
        import SHAPERSTUDY_utils
        SHAPERSTUDY_utils.findOrCreateComponent()
        anEngine = SHAPERSTUDY_utils.getEngine()

        # iterate all parts and all results to publish them in SHAPER_STUDY
        aSession = ModelAPI.ModelAPI_Session.get()
        aPartSet = aSession.moduleDocument()
        for aPartId in range(aPartSet.size(model.ModelAPI_ResultPart_group())):
          aPartObject = aPartSet.object(model.ModelAPI_ResultPart_group(), aPartId)
          aPart = ModelAPI.modelAPI_ResultPart(ModelAPI.modelAPI_Result(aPartObject)).partDoc()
          for aResId in range(aPart.size(model.ModelAPI_ResultBody_group())):
            aResObject = aPart.object(model.ModelAPI_ResultBody_group(), aResId)
            aRes = model.objectToResult(aResObject)
            print("Found a result to publish ", aRes.data().name())
            aSShape = anEngine.CreateShape()
            aSShape.SetShapeByStream(aRes.shape().getShapeStream())
            anEngine.AddInStudy(aSShape, aRes.data().name(), None)

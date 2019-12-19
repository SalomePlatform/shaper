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

import SHAPERSTUDY_ORB

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
        aSession = ModelAPI.ModelAPI_Session.get()
        aPartSet = aSession.moduleDocument()
        # check that the PartSet document current feature is the last to avoid problems with all
        # features update
        if aPartSet.size(model.ModelAPI_Feature_group()) > 0:
          aLastFeature = ModelAPI.objectToFeature(aPartSet.object(model.ModelAPI_Feature_group(), aPartSet.size(model.ModelAPI_Feature_group()) - 1))
          aCurrentFeature = aPartSet.currentFeature(True)
          if aLastFeature.data().featureId() != aCurrentFeature.data().featureId():
            EventsAPI.Events_InfoMessage("PublishToStudy", "Not all PartSet parts are up-to-date, nothing is published. Please, make the last PartSet feature as current.", self).send()
            return
        # find a shaper-study component
        salome.salome_init(1)
        import SHAPERSTUDY_utils
        aComponent = SHAPERSTUDY_utils.findOrCreateComponent()
        anEngine = SHAPERSTUDY_utils.getEngine()
        # collect all processed internal entries to break the link of unprocessed later
        allProcessed = []

        # iterate all parts and all results to publish them in SHAPER_STUDY
        for aPartId in range(aPartSet.size(model.ModelAPI_ResultPart_group())):
          aPartObject = aPartSet.object(model.ModelAPI_ResultPart_group(), aPartId)
          aPartRes = ModelAPI.modelAPI_ResultPart(ModelAPI.modelAPI_Result(aPartObject))
          aPartDoc = aPartRes.partDoc()
          if aPartDoc is None and aPartObject is not None:
            EventsAPI.Events_InfoMessage("PublishToStudy", "For publish to SHAPER-STUDY some Part is not activated", self).send()
            break
          aPartFeatureId = aPartSet.feature(aPartRes).data().featureId()
          for aResId in range(aPartDoc.size(model.ModelAPI_ResultBody_group())):
            aResObject = aPartDoc.object(model.ModelAPI_ResultBody_group(), aResId)
            aRes = model.objectToResult(aResObject)
            print("Found a result to publish ", aRes.data().name())
            aResFeatureId = aPartDoc.feature(aRes).data().featureId()
            aSSEntry = str(aPartFeatureId) + ":" + str(aResFeatureId)
            aSShape = anEngine.FindOrCreateShape(aSSEntry)
            aSShape.SetShapeByStream(aRes.shape().getShapeStream(False))
            if not aSShape.GetSO(): # publish in case it is a new shape
              anEngine.AddInStudy(aSShape, aRes.data().name(), None)
            allProcessed.append(aSSEntry)

        # process all SHAPER-STUDY shapes to find dead
        deadNames = []
        aSOIter = SHAPERSTUDY_utils.getStudy().NewChildIterator(aComponent)
        while aSOIter.More():
          aSO = aSOIter.Value()
          anIOR = aSO.GetIOR()
          anObj = salome.orb.string_to_object(anIOR)
          if isinstance(anObj, SHAPERSTUDY_ORB._objref_SHAPER_Object):
            anEntry = anObj.GetEntry()
            if len(anEntry) == 0:
              continue;
            elif anEntry.startswith("dead"):
              deadNames.append(anObj.GetEntry())
            elif anEntry not in allProcessed: # found a removed shape: make it dead
              anIndex = 1
              aName = "dead" + str(anIndex) + "_" + anEntry
              while aName in deadNames:
                anIndex = anIndex + 1
                aName = "dead" + str(anIndex) + "_" + anEntry
              anObj.SetEntry(aName)
              aSO.SetAttrString("AttributeName", aSO.GetName() + " (" + str(anIndex) + ")")
              # remove the reference - red node
              aSOIter2 = SHAPERSTUDY_utils.getStudy().NewChildIterator(aSO)
              while aSOIter2.More():
                aSO2 = aSOIter2.Value()
                if aSO2.ReferencedObject()[0] and aSO2.ReferencedObject()[1].GetID() == aSO.GetID():
                  aBuilder = SHAPERSTUDY_utils.getStudy().NewBuilder()
                  aBuilder.RemoveReference(aSO2)
                aSOIter2.Next()
          aSOIter.Next()

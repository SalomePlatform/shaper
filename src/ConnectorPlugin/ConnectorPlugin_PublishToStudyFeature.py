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
from GeomAPI import *
import GeomAlgoAPI

import salome
from salome.shaper import model

import SHAPERSTUDY_ORB
import SHAPERSTUDY_utils

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
            aResFeatureId = aPartDoc.feature(aRes).data().featureId()
            aSSEntry = str(aPartFeatureId) + ":" + str(aResFeatureId)
            aSShape = anEngine.FindOrCreateShape(aSSEntry)
            aSShape.SetShapeByStream(aRes.shape().getShapeStream(False))
            if not aSShape.GetSO(): # publish in case it is a new shape
              anEngine.AddInStudy(aSShape, aRes.data().name(), None)
            else: # restore a red reference if it was deleted
              aDone, aSO2 = aSShape.GetSO().FindSubObject(1)
              if aDone:
                aDone, aRef = aSO2.ReferencedObject()
                if not aDone:
                  aBuilder = SHAPERSTUDY_utils.getStudy().NewBuilder()
                  aBuilder.Addreference(aSO2, aSShape.GetSO())
            allProcessed.append(aSSEntry)
            # Groups
            self.processGroups(aRes, anEngine, aPartFeatureId, aSShape, False)
            # Fields
            self.processGroups(aRes, anEngine, aPartFeatureId, aSShape, True)

        # process all SHAPER-STUDY shapes to find dead
        aSOIter = SHAPERSTUDY_utils.getStudy().NewChildIterator(aComponent)
        while aSOIter.More():
          aSO = aSOIter.Value()
          aSOIter.Next() ### here because there is continue inside the loop!
          anIOR = aSO.GetIOR()
          if len(anIOR):
            anObj = salome.orb.string_to_object(anIOR)
            if isinstance(anObj, SHAPERSTUDY_ORB._objref_SHAPER_Object):
              anEntry = anObj.GetEntry()
              if len(anEntry) == 0:
                continue;
              elif anEntry not in allProcessed: # found a removed shape: make it dead for the moment
                # remove the reference - red node
                aRes, aSO2 = aSO.FindSubObject(1)
                if aRes:
                  aRes, aRef = aSO2.ReferencedObject()
                  if aRes:
                    aBuilder = SHAPERSTUDY_utils.getStudy().NewBuilder()
                    aBuilder.RemoveReference(aSO2)

    # Part of the "execute" method: processes the Groups of theRes result publication.
    # If theFields is true, the same is performed for Fields.
    def processGroups(self, theRes, theEngine, thePartFeatureId, theStudyShape, theFields):
      allGroupsProcessed = []
      if theFields:
        aRefGroups = ModelAPI.referencedFeatures(theRes, "Field", True)
      else:
        aRefGroups = ModelAPI.referencedFeatures(theRes, "Group", True)
      for aRef in aRefGroups:
        aGroupIndices = []
        aGroupHasIndex = {}
        aResShape = theRes.shape()
        if theFields:
          aSelList = aRef.selectionList("selected")
        else:
          aSelList = aRef.selectionList("group_list")
        aSelType = GeomAPI_Shape.shapeTypeByStr(aSelList.selectionType())
        for aSelIndex in range(aSelList.size()):
          aSelection = aSelList.value(aSelIndex)
          if aSelection.contextObject():
            aShape = aSelection.value()
            if aShape:
              allShapesList = [] # collect all sub-shapes selected in the group
              if aShape.shapeType() == 0: # compound
                anExplorer = GeomAPI_ShapeExplorer(aShape, aSelType)
                while anExplorer.more():
                  allShapesList.append(anExplorer.current())
                  anExplorer.next()
              else:
                allShapesList.append(aShape)
              # get index of each selected shape: if 0, this sub-shape is not in our result
              for aSelected in allShapesList:
                anId = GeomAlgoAPI.GeomAlgoAPI_CompoundBuilder.id(aResShape, aSelected)
                if anId > 0 and not anId in aGroupHasIndex:
                  aGroupIndices.append(anId)
                  aGroupHasIndex[anId] = 0
        if len(aGroupIndices): # create group
          aGroupFeatureId = aRef.data().featureId()
          if theFields:
            aFieldOp = theEngine.GetIFieldOperations()
            aGroupEntry = "field" + str(thePartFeatureId) + ":" + str(aGroupFeatureId)
            aGroup = aFieldOp.FindField(theStudyShape, aGroupEntry)
          else:
            aGroupOp = theEngine.GetIGroupOperations()
            aGroupEntry = "group" + str(thePartFeatureId) + ":" + str(aGroupFeatureId)
            aGroup = aGroupOp.FindGroup(theStudyShape, aGroupEntry)
          if not aGroup: # create a new
            if theFields:
              aGroup = aFieldOp.CreateFieldByType(theStudyShape, aSelType)
            else:
              aGroup = aGroupOp.CreateGroup(theStudyShape, aSelType)
            aGroup.SetEntry(aGroupEntry)
            theEngine.AddInStudy(aGroup, aRef.firstResult().data().name(), theStudyShape)
          aGroup.SetSelection(aGroupIndices)
          if theFields:
            self.fillField(aGroup, aRef, theEngine, aGroupIndices)
          # a group takes shape from the main result
          #aGroup.SetShapeByStream(aRef.firstResult().shape().getShapeStream(False)) # group shape
          allGroupsProcessed.append(aGroupEntry)
      # check all existing groups: if some does not processed, remove it from the tree
      aSOIter = SHAPERSTUDY_utils.getStudy().NewChildIterator(theStudyShape.GetSO())
      while aSOIter.More():
        aSO = aSOIter.Value()
        anIOR = aSO.GetIOR()
        if len(anIOR):
          anObj = salome.orb.string_to_object(anIOR)
          if (theFields and isinstance(anObj, SHAPERSTUDY_ORB._objref_SHAPER_Field)) or \
             (not theFields and type(anObj) == SHAPERSTUDY_ORB._objref_SHAPER_Group):
            anEntry = anObj.GetEntry()
            if anEntry not in allGroupsProcessed: # found a removed group => remove
              aBuilder = SHAPERSTUDY_utils.getStudy().NewBuilder()
              aBuilder.RemoveObject(anObj.GetSO())
        aSOIter.Next()

    # Part of the "execute" method: theFiled fields filling.
    def fillField(self, theField, theFeature, theEngine, theSelectionIndices):
      aTables = theFeature.tables("values")
      aValType = aTables.type() # type of the values
      theField.SetValuesType(aValType)
      aNumSteps = aTables.tables() # number of steps is number of tables
      aSteps = []
      for aVal in range(aNumSteps):
        aSteps.append(aVal + 1)
      theField.SetSteps(aSteps)
      aCompNames = []
      aCompNamesAttr = theFeature.stringArray("components_names")
      for anIndex in range(aCompNamesAttr.size()):
        aCompNames.append(aCompNamesAttr.value(anIndex))
      theField.SetComponents(aCompNames)
      # prepare the sub-shapes indices: all values for all sub-shapes must be defined
      aShapeOp = theEngine.GetIShapesOperations()
      allIndices = aShapeOp.GetAllSubShapesIDs(theField.GetShape(), theField.GetSelectionType(), False)
      # define steps
      theField.ClearFieldSteps()
      for aVal in range(aNumSteps):
        aVals = []
        for aCol in range(aTables.columns()):
          #for aRow in range(aTables.rows()):
          for anIndex in allIndices:
            if anIndex in theSelectionIndices:
              aRow = theSelectionIndices.index(anIndex) + 1 # starting from the first line
            else:
              aRow = 0 # default value
            aStrVal = aTables.valueStr(aRow, aCol, aVal)
            if aValType == 0: # boolean
              if aStrVal == "True":
                aVals.append(1)
              else:
                aVals.append(0)
            elif aValType == 1: # int
              aVals.append(int(aStrVal))
            elif aValType == 2: # double
              aVals.append(float(aStrVal))
        theField.AddFieldStep(theFeature.intArray("stamps").value(aVal), aVal + 1, aVals)

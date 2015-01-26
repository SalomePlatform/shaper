"""
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

import EventsAPI
import ModelAPI

import salome
from salome.geom import geomBuilder


class ExportFeature(ModelAPI.ModelAPI_Feature):

    "Feature to create a box by drawing a sketch and extruding it"

    def __init__(self):
        ModelAPI.ModelAPI_Feature.__init__(self)

    @staticmethod
    def ID():
        return "ExportToGEOM"

    def getKind(self):
        return ExportFeature.ID()

    # This feature is action: has no property pannel and executes immideately
    # def isAction(self):
    #    return True

    def isInHistory(self):
        return False

    def initAttributes(self):
        # This feature has no attributes, but should perform some actions on initialization
        aSession = ModelAPI.ModelAPI_Session.get()
        self.Part = aSession.activeDocument()
        self.geomObjects = []
        self.geompy = geomBuilder.New(salome.myStudy)
       
        # Export bodies and groups
        self.exportBodies()
        self.exportGroups()
        
    def exportBodies(self):
        # Get all bodies
        kResultBodyType = "Bodies"
        aPartSize = self.Part.size(kResultBodyType)
        if aPartSize == 0:
            EventsAPI.Events_Error_send("No results in the active document")
            return
          
        anObjList = [self.Part.object(kResultBodyType, idx) for idx in xrange(aPartSize)]     
        for idx, anObject in enumerate(anObjList):
            aResult = ModelAPI.modelAPI_Result(anObject)
            aBodyResult = ModelAPI.modelAPI_ResultBody(aResult)
            if not aBodyResult:
                continue
            aShape = aBodyResult.shape()
            aDump = aShape.getShapeStream()
            # Load shape to SALOME Geom
            aBrep = self.geompy.RestoreShape(aDump)
            self.geompy.addToStudy(aBrep, "NewGeomShape_{0}".format(idx + 1))
            self.geomObjects.append([aShape, aBrep])

    def exportGroups(self):
        # iterate all features to find groups
        aFeaturesNum = self.Part.size("Features")
        groupIndex = 0
        for anIndex in range(0, aFeaturesNum):
            aFeature = self.Part.object("Features", anIndex)
            aSelectionList = aFeature.data().selectionList("group_list")
            # if a group has been found
            if aSelectionList:
                groupIndex = groupIndex + 1
                self.createGroupFromList(aSelectionList, "NewGeomGroup_{0}".format(groupIndex))
                     
    def createGroupFromList(self, theSelectionList, theGroupName):     
        # iterate on all selected entities of the group
        # and get the corresponding ID
        aSelectionNum = theSelectionList.size()
        Ids = []
        for aSelIndex in range(0, aSelectionNum):
            aSelection = theSelectionList.value(aSelIndex)
            aSelectionContext = aSelection.context()
            anID = aSelection.Id()
            Ids.append(anID)
            if aSelection.value().isVertex():
                groupType = "VERTEX"
            elif aSelection.value().isEdge():
                groupType = "EDGE" 
            elif aSelection.value().isFace():
                groupType = "FACE"
            else:
                groupType = "SOLID"

        # iterate on exported objects and check if the current
        # group refers to this object
        for obj in self.geomObjects: 
            if aSelectionContext.shape().isEqual(obj[0]):
                aGroup = self.geompy.CreateGroup(obj[1], self.geompy.ShapeType[groupType])
                self.geompy.UnionIDs(aGroup,Ids)
                self.geompy.addToStudyInFather(obj[1], aGroup, theGroupName)
          
    def execute(self):
        # Nothing to execute: all logic would be in the initAttributes
        pass

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
        # This feature has no attributes, but should perfore some actions on initialization
        aSession = ModelAPI.ModelAPI_Session.get()
        aPart = aSession.activeDocument()
        # Get all bodies
        kResultBodyType = "Bodies"
        aPartSize = aPart.size(kResultBodyType)
        if aPartSize == 0:
            EventsAPI.Events_Error_send("No results in the active document")
            return

        anObjList = [aPart.object(kResultBodyType, idx) for idx in xrange(aPartSize)]
        for idx, anObject in enumerate(anObjList):
            aResult = ModelAPI.modelAPI_Result(anObject)
            aBodyResult = ModelAPI.modelAPI_ResultBody(aResult)
            if not aBodyResult:
                continue
            aShape = aBodyResult.shape()
            aDump = aShape.getShapeStream()
            # Load shape to SALOME Geom
            geompy = geomBuilder.New(salome.myStudy)
            aBrep = geompy.RestoreShape(aDump)
            geompy.addToStudy(aBrep, "NewGeomShape_{0}".format(idx))

    def execute(self):
        # Nothing to execute: all logic would be in the initAttributes
        pass

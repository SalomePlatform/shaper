"""
Copyright (C) 2014-20xx CEA/DEN, EDF R&D
"""

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
    def isAction(self):
        return True

    def initAttributes(self):
        # This feature has no attributes, but should perfore some actions on initialization
        aSession = ModelAPI.ModelAPI_Session.get()
        aPart = aSession.activeDocument()
        # Get all bodies
        kResultBodyType = "ResultBody"
        aPartSize = aPart.size(kResultBodyType)
        if aPartSize == 0:
            print "No results in the active document"
            return

        aResultList = [aPart.object(kResultBodyType, idx) for idx in xrange(aPartSize)]
        for idx, aResult in enumerate(aResultList):
            aBodyResult = modelAPI_ResultBody(aResult)
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

# TEST
"""
if __name__=='__main__':
  session = ModelAPI.ModelAPI_Session.get()
  part = session.activeDocument()
  session.startOperation()
  feature = part.addFeature('Box')
  feature.real('box_width').setValue(10)
  feature.real('box_length').setValue(10)
  feature.real('box_height').setValue(10)
  feature.execute()
  session.finishOperation()
"""

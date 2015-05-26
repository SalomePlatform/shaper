"""
      TestImport.py
      Unit test of ExchangePlugin_TestImport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import os
import math

__updated__ = "2015-05-22"

aSession = ModelAPI_Session.get()
#=========================================================================
# Common test function
#=========================================================================
def testImport(theType, theFile, theVolume, theDelta):
    # Create a part for import
    aSession.startOperation()
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()
    
    aSession.startOperation()
    aFeatureKind = "Import"
    anImportFeature = aPart.addFeature(aFeatureKind)
    assert anImportFeature, "{0}: Can not create a feature {1}".format(theType, aFeatureKind)
    aFieldName = "import_file_selector"
    file = anImportFeature.string(aFieldName)
    assert file, "{0}: Can not receive string field {1}".format(theType, aFieldName)
    file.setValue(theFile)
    anImportFeature.execute()
    aSession.finishOperation()
    
    # Check results
    assert anImportFeature.error() == '', "{0}: The error after execution: {1}".format(theType, anImportFeature.error())
    assert len(anImportFeature.results()) == 1, "{0}: Wrong number of results: expected = 1, real = {1}".format(theType, len(anImportFeature.results()))
    aResultBody = modelAPI_ResultBody(anImportFeature.firstResult())
    assert aResultBody, "{0}: The result is not a body".format(theType)
    aShape = aResultBody.shape()
    assert aShape, "{0}: The body does not have a shape".format(theType)
    
    # Check shape volume
    aRefVolume = theVolume
    aResVolume = GeomAlgoAPI_ShapeProps.volume(aShape)
    assert (math.fabs(aResVolume - aRefVolume) < theDelta), "{0}: The volume is wrong: expected = {1}, real = {2}".format(theType, aRefVolume, aResVolume)

if __name__ == '__main__':
#=========================================================================
# Create a shape imported from BREP
#=========================================================================
    testImport("BREP", "Data/solid.brep", 259982.29715, 10 ** -5)
#=========================================================================
# Create a shape imported from STEP
#=========================================================================
    testImport("STP", "Data/screw.stp", 3.78827059338e-06, 10 ** -17)
    testImport("STEP", "Data/screw.step", 3.78827059338e-06, 10 ** -17)
#=========================================================================
# Create a shape imported from IGES
#=========================================================================
    testImport("IGES", "Data/bearing.iges", 6.86980756235e-14, 10 ** -25)
#=========================================================================
# End of test
#=========================================================================

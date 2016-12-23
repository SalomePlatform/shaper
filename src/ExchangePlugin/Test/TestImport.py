"""
      TestImport.py
      Unit test of ExchangePlugin_TestImport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomAlgoAPI import *
import math

__updated__ = "2015-05-22"

aSession = ModelAPI_Session.get()
#=========================================================================
# Common test function
#=========================================================================
def testImport(theType, theFile, theVolume, theDelta):
    # Create a part for import
    aSession.startOperation("Create part for import")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import file")
    aFeatureKind = "Import"
    anImportFeature = aPart.addFeature(aFeatureKind)
    assert anImportFeature, "{0}: Can not create a feature {1}".format(theType, aFeatureKind)
    aFieldName = "file_path"
    file = anImportFeature.string(aFieldName)
    assert file, "{0}: Can not receive string field {1}".format(theType, aFieldName)
    file.setValue(theFile)
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
    aResVolume = GeomAlgoAPI_ShapeTools.volume(aShape)
    assert (math.fabs(aResVolume - aRefVolume) < theDelta), "{0}: The volume is wrong: expected = {1}, real = {2}".format(theType, aRefVolume, aResVolume)

def testImportXAO():
    # Create a part for import
    aSession.startOperation("Create part for import")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import XAO")
    anImportFeature = aPart.addFeature("Import")
    anImportFeature.string("file_path").setValue("Data/test.xao")
    aSession.finishOperation()

    # Check results
    assert anImportFeature.error() == ''
    assert anImportFeature.name() == "mygeom"
    assert len(anImportFeature.results()) == 1
    assert modelAPI_ResultBody(anImportFeature.firstResult())
    assert anImportFeature.firstResult().data().name() == "mygeom_1"
    aCompositeFeature = featureToCompositeFeature(anImportFeature)
    # Two groups and one field
    assert aCompositeFeature.numberOfSubs(False) == 3

    aFeature1 = aCompositeFeature.subFeature(0, False)
    assert aFeature1.getKind() == "Group"
    assert aFeature1.name() == "boite_1"

    aSelectionList = aFeature1.selectionList("group_list")
    assert aSelectionList.selectionType() == "solid"
    assert aSelectionList.size() == 1
    assert aSelectionList.value(0).namingName("") == "mygeom_1"

    aFeature2 = aCompositeFeature.subFeature(1, False)
    assert aFeature2.getKind() == "Group"
    assert aFeature2.name() == "Group_2"

    aSelectionList = aFeature2.selectionList("group_list") 
    assert aSelectionList.selectionType() == "face"
    assert aSelectionList.size() == 2
    assert aSelectionList.value(0).namingName("") == "mygeom_1/Shape1"
    print aSelectionList.value(1).namingName("")
    assert aSelectionList.value(1).namingName("") == "mygeom_1/Shape2"

    aFeature3 = aCompositeFeature.subFeature(2, False)
    assert aFeature3.getKind() == "Field"
    assert aFeature3.name() == "color"
    assert aFeature3.intArray("stamps").size() == 2
    assert aFeature3.tables("values").rows() == 2
    assert aFeature3.tables("values").columns() == 3
    assert aFeature3.tables("values").tables() == 2
    assert aFeature3.tables("values").type() == 1 # integer
    assert aFeature3.selectionList("selected").size() == 1

if __name__ == '__main__':
#=========================================================================
# Create a shape imported from BREP
#=========================================================================
    testImport("BREP", "Data/solid.brep", 259982.297176, 10 ** -5)
    testImport("BRP", "Data/solid.brp", 259982.297176, 10 ** -5)
#=========================================================================
# Create a shape imported from STEP
#=========================================================================
    testImport("STP", "Data/screw.stp", 3.78827401738e-06, 10 ** -17)
    testImport("STEP", "Data/screw.step", 3.78827401738e-06, 10 ** -17)
#=========================================================================
# Create a shape imported from IGES
#=========================================================================
    testImport("IGES", "Data/bearing.iges", 6.86970803067e-14, 10 ** -25)
    testImport("IGS", "Data/bearing.igs", 6.86970803067e-14, 10 ** -25)
#=========================================================================
# Create a shape imported from XAO
#=========================================================================
    testImportXAO()
#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())

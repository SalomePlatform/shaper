"""
      TestExport.py
      Unit test of ExchangePlugin_TestExport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================
import os
import math

from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

from TestImport import testImport

__updated__ = "2015-05-22"

aSession = ModelAPI_Session.get()
#=========================================================================
# Help functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)

#=========================================================================
# Common test function
#=========================================================================
def testExport(theType, theFormat, theFile, theVolume, theDelta):
    # Import a reference part 
    aSession.startOperation()
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()
    
    aSession.startOperation()
    anImportFeature = aPart.addFeature("Import")
    anImportFeature.string("import_file_selector").setValue("Data/screw.step")
    anImportFeature.execute()
    aSession.finishOperation()
    
    removeFile(theFile)
    # Export a part
    aSession.startOperation()
    aFeatureKind = "Export"
    anExportFeature = aPart.addFeature(aFeatureKind)
    assert anExportFeature, "{0}: Can not create a feature {1}".format(theType, aFeatureKind)
    
    aFormatAttrName = "export_file_format"
    aFormatAttr = anExportFeature.string(aFormatAttrName)
    assert aFormatAttr, "{0}: Can not receive string field {1}".format(theType, aFormatAttrName)
    aFormatAttr.setValue(theFormat)
    
    aFileAttrName = "export_file_selector"
    aFileAttr = anExportFeature.string(aFileAttrName)
    assert aFileAttr, "{0}: Can not receive string field {1}".format(theType, aFileAttrName)
    aFileAttr.setValue(theFile)
    
    aSelectionListAttrName = "selection_list"
    aSelectionListAttr = anExportFeature.selectionList(aSelectionListAttrName)
    assert aSelectionListAttr, "{0}: Can not receive selection list field {1}".format(theType, aSelectionListAttrName)
    aSelectionListAttr.setSelectionType("solids")
    aSelectionListAttr.append(anImportFeature.firstResult(), anImportFeature.firstResult().shape())
    
    anExportFeature.execute()
    aSession.finishOperation()
    
    assert os.path.exists(theFile), "{0}: Can not find exported file {1}".format(theType, theFile)
    
    # Test exported file by importing
    testImport(theType, theFile, theVolume, theDelta)

if __name__ == '__main__':
#=========================================================================
# Export a shape into BREP
#=========================================================================
    aRealVolume = 3.78827059338e-06
    testExport("BREP", "BREP", os.path.join(os.getcwd(), "Data", "screw_export.brep"), aRealVolume, 10 ** -17)
#=========================================================================
# Export a shape into STEP
#=========================================================================
    testExport("STEP", "STEP", os.path.join(os.getcwd(), "Data", "screw_export.step"), 3.7882546512e-06, 10 ** -17)
    testExport("STP", "STEP", os.path.join(os.getcwd(), "Data", "screw_export.stp"), 3.7882546512e-06, 10 ** -17)
#=========================================================================
# Export a shape into IGES
#=========================================================================
    testExport("IGES-5.1", "IGES-5.1", os.path.join(os.getcwd(), "Data", "screw_export-5.1.iges"), 1.98291079746e-06, 10 ** -17)
    testExport("IGES-5.3", "IGES-5.3", os.path.join(os.getcwd(), "Data", "screw_export-5.3.iges"), 3.78827060085e-06, 10 ** -17)
#=========================================================================
# End of test
#=========================================================================

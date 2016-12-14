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

from TestImport import testImport

import model

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
    aSession.startOperation("Add part")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import screw")
    anImportFeature = aPart.addFeature("Import")
    anImportFeature.string("file_path").setValue("Data/screw.step")
    aSession.finishOperation()

    removeFile(theFile)
    # Export a part
    aSession.startOperation("Export part")
    anExportFeature = aPart.addFeature("Export")
    anExportFeature.string("file_format").setValue(theFormat)
    print "theFile=",theFile
    anExportFeature.string("file_path").setValue(theFile)
    anExportFeature.string("ExportType").setValue("Regular")
    aSelectionListAttr = anExportFeature.selectionList("selection_list")
    aSelectionListAttr.setSelectionType("solids")
    aSelectionListAttr.append(anImportFeature.firstResult(), anImportFeature.firstResult().shape())
    aSession.finishOperation()

    assert os.path.exists(theFile)

    # Test exported file by importing
    testImport(theType, theFile, theVolume, theDelta)

def testExportXAO():
    # Import a reference part
    aSession.startOperation("Add part")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import Box_1")
    anImportFeature = aPart.addFeature("Import")
    anImportFeature.string("file_path").setValue("Data/Box_1.brep")
    aSession.finishOperation()

    # Create groups
    aSession.startOperation("First group")
    aGroupFeature = aSession.activeDocument().addFeature("Group")
    aGroupFeature.data().setName("boite_1")
    aSelectionListAttr = aGroupFeature.selectionList("group_list")
    aSelectionListAttr.setSelectionType("solid")
    aSelectionListAttr.append(anImportFeature.lastResult(), None)
    aSession.finishOperation()

    aSession.startOperation("Second Group")
    aGroupFeature = aSession.activeDocument().addFeature("Group")
    aGroupFeature.data().setName("")
    aSelectionListAttr = aGroupFeature.selectionList("group_list")
    aSelectionListAttr.setSelectionType("face")
    aSelectionListAttr.append("Box_1_1/Shape1")
    aSelectionListAttr.append("Box_1_1/Shape2")
    aSession.finishOperation()

    aSession.startOperation("Create a field")
    aField = aSession.activeDocument().addFeature("Field")
    aSelectionListAttr = aField.selectionList("selected")
    aSelectionListAttr.setSelectionType("face")
    aSelectionListAttr.append("Box_1_1/Shape1")
    aSelectionListAttr.append("Box_1_1/Shape2")
    aComponentNames = aField.stringArray("components_names")
    aComponentNames.setSize(2) # two components
    aComponentNames.setValue(0, "temperatue")
    aComponentNames.setValue(1, "porosity")
    aStamps = aField.intArray("stamps")
    aStamps.setSize(1) # one step
    aStamps.setValue(0, 10)
    aTables = aField.tables("values")
    aTables.setType(2) # double
    aTables.setSize(1 + 2, 2, 1) # default row + number of selected, number of compoents, number of steps (tables)
    aTables.setValue(1., 0, 0, 0) # value, index of selection, index of component, index of step
    aTables.setValue(2., 1, 0, 0)
    aTables.setValue(3., 2, 0, 0)
    aTables.setValue(4., 0, 1, 0)
    aTables.setValue(5., 1, 1, 0)
    aTables.setValue(6., 2, 1, 0)
    aSession.finishOperation()

    # Export
    aSession.startOperation("Export to XAO")
    anExportFeature = aPart.addFeature("Export")
    anExportFeature.string("xao_file_path").setValue("Data/export.xao")
    anExportFeature.string("file_format").setValue("XAO")
    anExportFeature.string("ExportType").setValue("XAO")
    anExportFeature.string("xao_author").setValue("me")
    anExportFeature.string("xao_geometry_name").setValue("mygeom")
    aSession.finishOperation()

    # Check exported file
#    import filecmp
#    assert filecmp.cmp("Data/export.xao", "Data/export_ref.xao")

if __name__ == '__main__':
#=========================================================================
# Export a shape into BREP
#=========================================================================
    aRealVolume = 3.78827401738e-06
    testExport("BREP", "BREP", os.path.join(os.getcwd(), "Data", "screw_export.brep"), aRealVolume, 10 ** -17)
    testExport("BRP", "BREP", os.path.join(os.getcwd(), "Data", "screw_export.brp"), aRealVolume, 10 ** -17)
#=========================================================================
# Export a shape into STEP
#=========================================================================
    testExport("STEP", "STEP", os.path.join(os.getcwd(), "Data", "screw_export.step"), 3.78825807533e-06, 10 ** -17)
    testExport("STP", "STEP", os.path.join(os.getcwd(), "Data", "screw_export.stp"), 3.78825807533e-06, 10 ** -17)
#=========================================================================
# Export a shape into IGES
#=========================================================================
    testExport("IGES-5.1", "IGES-5.1", os.path.join(os.getcwd(), "Data", "screw_export-5.1.iges"), 3.78829613776e-06, 10 ** -17)
    testExport("IGS-5.1", "IGES-5.1", os.path.join(os.getcwd(), "Data", "screw_export-5.1.igs"), 3.78829613776e-06, 10 ** -17)
    testExport("IGES-5.3", "IGES-5.3", os.path.join(os.getcwd(), "Data", "screw_export-5.3.iges"), 3.78827401651e-06, 10 ** -17)
    testExport("IGS-5.3", "IGES-5.3", os.path.join(os.getcwd(), "Data", "screw_export-5.3.igs"), 3.78827401651e-06, 10 ** -17)
#=========================================================================
# Export a shape into XAO
#=========================================================================
    testExportXAO()
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())

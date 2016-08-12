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
    anImport = model.addImport(aPart, "Data/screw.step")
#     anImportFeature = aPart.addFeature("Import")
#     anImportFeature.string("file_path").setValue("Data/screw.step")
#     anImportFeature.execute()
    aSession.finishOperation()

    removeFile(theFile)
    # Export a part
    aSession.startOperation("Export part")
    model.exportToFile(aPart, theFile, anImport.result(), theFormat)
#     anExportFeature = aPart.addFeature("Export")
#     anExportFeature.string("file_format").setValue(theFormat)
#     anExportFeature.string("file_path").setValue(theFile)
#     aSelectionListAttr = anExportFeature.selectionList("selection_list")
#     aSelectionListAttr.setSelectionType("solids")
#     aSelectionListAttr.append(anImportFeature.firstResult(), anImportFeature.firstResult().shape())
#     anExportFeature.execute()
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
    anImport = model.addImport(aPart, "Data/Box_1.brep")
#     anImportFeature = aPart.addFeature("Import")
#     anImportFeature.string("file_path").setValue("Data/Box_1.brep")
#     anImportFeature.execute()
    aSession.finishOperation()

    # Create groups
    aSession.startOperation()
    aGroup = model.addGroup(aPart, anImport.result())
    aGroup.defaultResult().data().setName("boite_1")
    aGroup.groupList().setSelectionType("solid")
#     aGroupFeature = aSession.activeDocument().addFeature("Group")
#     aGroupFeature.data().setName("boite_1")
#     aSelectionListAttr = aGroupFeature.selectionList("group_list")
#     aSelectionListAttr.setSelectionType("solid")
#     aSelectionListAttr.append(anImport.feature().lastResult(), None)
#     aGroupFeature.execute()
    aSession.finishOperation()

    aSession.startOperation()
    aGroup = model.addGroup(aPart, [])
    aGroup.defaultResult().data().setName("")
    aGroup.groupList().setSelectionType("face")
    aGroup.groupList().append("Box_1_1/Shape1_1")
    aGroup.groupList().append("Box_1_1/Shape2_1")
#     aGroupFeature = aSession.activeDocument().addFeature("Group")
#     aGroupFeature.data().setName("")
#     aSelectionListAttr = aGroupFeature.selectionList("group_list")
#     aSelectionListAttr.setSelectionType("face")
#     aSelectionListAttr.append("Box_1_1/Shape1_1")
#     aSelectionListAttr.append("Box_1_1/Shape2_1")
#     aGroupFeature.execute()
    aSession.finishOperation()

    # Export
    aSession.startOperation("Export")
    model.exportToXAO(aPart, "Data/export.xao", "me", "mygeom")
#     anExportFeature = aPart.addFeature("Export")
# #     anExportFeature.string("ExportType").setValue("XAO")
#     anExportFeature.string("file_path").setValue("Data/export.xao")
# #     anExportFeature.string("file_format").setValue("XAO")
#     anExportFeature.string("xao_author").setValue("me")
#     anExportFeature.string("xao_geometry_name").setValue("mygeom")
#     anExportFeature.execute()
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

# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

"""
      TestExport.py
      Unit test of ExchangePlugin_TestExport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================
import os
import math
from tempfile import TemporaryDirectory

from ModelAPI import *

from TestImport import testImport

from salome.shaper import model

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
def testExport(theType, theFormat, theFile, theVolume, theDelta, theErrorExpected = False):
    # Import a reference part
    aSession.startOperation("Add part")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import screw")
    anImportFeature = aPart.addFeature("Import")
    aShapePath = os.path.join(os.getenv("DATA_DIR"), "Shapes", "Step", "screw.step")
    anImportFeature.string("file_path").setValue(aShapePath)
    aSession.finishOperation()

    removeFile(theFile)
    # Export a part
    aSession.startOperation("Export part")
    anExportFeature = aPart.addFeature("Export")
    anExportFeature.string("file_format").setValue(theFormat)
    print("theFile=",theFile)
    anExportFeature.string("file_path").setValue(theFile)
    anExportFeature.string("ExportType").setValue("Regular")
    aSelectionListAttr = anExportFeature.selectionList("selection_list")
    aSelectionListAttr.setSelectionType("solids")
    aSelectionListAttr.append(anImportFeature.firstResult(), anImportFeature.firstResult().shape())
    aSession.finishOperation()

    if theErrorExpected:
        assert anExportFeature.error() != ""
        aPart.removeFeature(anExportFeature)
    else:
        assert os.path.exists(theFile)

        # Test exported file by importing
        testImport(theType, theFile, theVolume, theDelta)

def testExportXAO(theFile, theEmptyFormat = False):
    type = "XAO"
    format = "XAO"
    if theEmptyFormat:
        type = "Regular"
        format = ""

    # Import a reference part
    aSession.startOperation("Add part")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import Box_1")
    anImportFeature = aPart.addFeature("Import")
    aShapePath = os.path.join(os.getenv("DATA_DIR"), "Shapes", "Brep", "box1.brep")
    anImportFeature.string("file_path").setValue(aShapePath)
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
    aSelectionListAttr.append("box1_1/Shape_1")
    aSelectionListAttr.append("box1_1/Shape_2")
    aSession.finishOperation()

    aSession.startOperation("Create a field")
    aField = aSession.activeDocument().addFeature("Field")
    aSelectionListAttr = aField.selectionList("selected")
    aSelectionListAttr.setSelectionType("face")
    aSelectionListAttr.append("box1_1/Shape_1")
    aSelectionListAttr.append("box1_1/Shape_2")
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
    anExportFeature.string("xao_file_path").setValue(theFile)
    anExportFeature.string("file_format").setValue(type)
    anExportFeature.string("ExportType").setValue(type)
    anExportFeature.string("xao_author").setValue("me")
    anExportFeature.string("xao_geometry_name").setValue("mygeom")
    aSession.finishOperation()

    # Check exported file
    aRefPath = os.path.join(os.getenv("DATA_DIR"), "Shapes", "Xao", "box2.xao")
    # endlines may be different on different platforms, thus compare files line-by-line
    areFilesEqual = True
    with open(theFile, 'r') as file, open(aRefPath, 'r') as ref:
        l1 = l2 = True
        while l1 and l2 and areFilesEqual:
            l1 = file.readline()
            l2 = ref.readline()
            areFilesEqual = l1 == l2
    assert areFilesEqual

if __name__ == '__main__':
    with TemporaryDirectory() as tmp_dir:
        #=========================================================================
        # Export a shape into BREP
        #=========================================================================
        aRealVolume = 3.78827401738e-06
        testExport("BREP", "BREP", os.path.join(tmp_dir, "screw_export.brep"), aRealVolume, 10 ** -17)
        testExport("BRP", "BREP", os.path.join(tmp_dir, "screw_export.brp"), aRealVolume, 10 ** -17)
        testExport("Regular", "", os.path.join(tmp_dir, "screw_export.brep"), aRealVolume, 10 ** -17)
        #=========================================================================
        # Export a shape into STEP
        #=========================================================================
        testExport("STEP", "STEP", os.path.join(tmp_dir, "screw_export.step"), 3.788258075329978e-06, 10 ** -17)
        testExport("STP", "STEP", os.path.join(tmp_dir, "screw_export.stp"), 3.788258075329978e-06, 10 ** -17)
        testExport("Regular", "", os.path.join(tmp_dir, "screw_export.step"), 3.788258075329978e-06, 10 ** -17)
        #=========================================================================
        # Export a shape into IGES
        #=========================================================================
        testExport("IGES-5.1", "IGES-5.1", os.path.join(tmp_dir, "screw_export-5.1.iges"), 0.0019293313766693052, 10 ** -17)
        testExport("IGS-5.1", "IGES-5.1", os.path.join(tmp_dir, "screw_export-5.1.igs"), 0.0019293313766693052, 10 ** -17)
        testExport("Regular", "", os.path.join(tmp_dir, "screw_export-5.1.iges"), 0.0019293313766693052, 10 ** -17)
        testExport("IGES-5.3", "IGES-5.3", os.path.join(tmp_dir, "screw_export-5.3.iges"), 3.78827401651e-06, 10 ** -17)
        testExport("IGS-5.3", "IGES-5.3", os.path.join(tmp_dir, "screw_export-5.3.igs"), 3.78827401651e-06, 10 ** -17)
        #=========================================================================
        # Export a shape into XAO
        #=========================================================================
        testExportXAO(os.path.join(tmp_dir, "export.xao"))
        testExportXAO(os.path.join(tmp_dir, "export.xao"), True)
        #=========================================================================
        # Check error when export to unsupported format
        #=========================================================================
        testExport("Regular", "", os.path.join(tmp_dir, "screw_export.dwg"), 3.78825807533e-06, 10 ** -17, True)
        #=========================================================================
        # End of test
        #=========================================================================

        assert(model.checkPythonDump())

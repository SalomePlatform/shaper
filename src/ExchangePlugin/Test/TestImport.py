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
      TestImport.py
      Unit test of ExchangePlugin_TestImport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomAlgoAPI import *

import os
import math
import shutil
from tempfile import TemporaryDirectory

__updated__ = "2015-05-22"

aSession = ModelAPI_Session.get()

#=========================================================================
# Common test function
#=========================================================================
def getShapePath(path):
    shapes_dir = os.path.join(os.getenv("DATA_DIR"), "Shapes")
    return os.path.join(shapes_dir, path)

def testImport(theType, theFile, theVolume, theDelta, theErrorExpected = False):
    # Create a part for import
    aSession.startOperation("Create part for import")
    aPartFeature = aSession.moduleDocument().addFeature("Part")
    aSession.finishOperation()
    aPart = aSession.activeDocument()

    aSession.startOperation("Import file")
    aFeatureKind = "Import"
    anImportFeature = aPart.addFeature(aFeatureKind)
    assert anImportFeature, "{0}: Can not create a feature {1}".format(theType, aFeatureKind)
    if theType == "STP" or theType == "STEP":
        aFieldName = "step_file_path"  
        file = anImportFeature.string(aFieldName)
        assert file, "{0}: Can not receive string field {1}".format(theType, aFieldName)
        file.setValue(theFile)
        aFieldName = "step_scale_inter_units"
        units = anImportFeature.boolean(aFieldName)
        assert units, "{0}: Can not receive string field {1}".format(theType, aFieldName)
        units.setValue(True)
    aFieldName = "file_path"
    file = anImportFeature.string(aFieldName)
    assert file, "{0}: Can not receive string field {1}".format(theType, aFieldName)
    file.setValue(theFile)
    aFieldName = "ImportType"
    type = anImportFeature.string(aFieldName)
    assert type, "{0}: Can not receive string field {1}".format(theType, aFieldName)
    type.setValue(theType)
    aSession.finishOperation()

    if theErrorExpected:
        assert anImportFeature.error() != ''
    else:
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
    anImportFeature.string("file_path").setValue(getShapePath("Xao/box1.xao"))
    aFieldName = "ImportType"
    type = anImportFeature.string(aFieldName)
    type.setValue("XAO")
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
    assert aSelectionList.value(0).namingName("") == "mygeom_1/Shape_1"
    assert aSelectionList.value(1).namingName("") == "mygeom_1/Shape_2"

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
    with TemporaryDirectory() as tmp_dir:
        #=========================================================================
        # Create a shape imported from BREP
        #=========================================================================
        shape_path = getShapePath("Brep/solid.brep")
        testImport("BREP", shape_path, 259982.297176, 10 ** -5)
        shape_path = shutil.copyfile(shape_path, os.path.join(tmp_dir, "solid.brp"))
        testImport("BRP", shape_path, 259982.297176, 10 ** -5)
        #=========================================================================
        # Create a shape imported from STEP
        #=========================================================================
        shape_path = getShapePath("Step/screw.step")
        testImport("STEP", shape_path, 3.78827401738e-06, 10 ** -17)
        shape_path = shutil.copyfile(shape_path, os.path.join(tmp_dir, "screw.stp"))
        testImport("STEP", shape_path, 3.78827401738e-06, 10 ** -17)
        #=========================================================================
        # Create a shape imported from IGES
        #=========================================================================
        shape_path = getShapePath("Iges/bearing.igs")
        testImport("IGS", shape_path, 1.3407098545036494e-08, 10 ** -25)
        shape_path = shutil.copyfile(shape_path, os.path.join(tmp_dir, "bearing.iges"))
        testImport("IGES", shape_path, 1.3407098545036494e-08, 10 ** -25)

        #=========================================================================
        # Create a shape imported from XAO
        #=========================================================================
        testImportXAO()

        #=========================================================================
        # End of test
        #=========================================================================

        from salome.shaper import model
        assert(model.checkPythonDump())

        #=========================================================================
        # Check import errors
        #=========================================================================
        testImport("BREP", "", 0, 10 ** -25, True)
        shape_path = getShapePath("Brep/solid.dwg")
        testImport("BREP", shape_path, 0, 10 ** -25, True)
        shape_path = getShapePath("Xao/wrong_file.xao")
        testImport("XAO", shape_path, 0, 10 ** -25, True)

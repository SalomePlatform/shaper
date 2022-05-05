# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
      TestImportStep.py
      Unit test of ExchangePlugin_ImportFeature class for STEP 
"""
#=========================================================================
# Initialization of the test
#=========================================================================

import salome

import os
import math
from tempfile import TemporaryDirectory

from ModelAPI import *

from salome.shaper import model

from GeomAPI import GeomAPI_Shape

from GeomAlgoAPI import *

__updated__ = "2015-05-22"

salome.standalone()
salome.salome_init(1)

#=========================================================================
# Help functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)

#=========================================================================
# test Import STEP 
#=========================================================================
def testImportSTEP():

    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    aShapePath = os.path.join(os.getenv("DATA_DIR"), "Shapes", "Step", "black_and_white.step")
    print("aShapePath=",aShapePath)
    Import_1 = model.addImportSTEP(Part_1_doc,aShapePath, True, True, True)

    model.do()

    # Check results
    Import_1_Feature = Import_1.feature()
    assert Import_1_Feature.error() == ''
    assert Import_1_Feature.name() == "black_and_white"
    assert len(Import_1_Feature.results()) == 1
    model.testNbSubShapes(Import_1, GeomAPI_Shape.SOLID, [2])

    aCompositeFeature = featureToCompositeFeature(Import_1_Feature)
    assert aCompositeFeature.numberOfSubs(False) == 4

    aFeature1 = aCompositeFeature.subFeature(0, False)
    assert aFeature1.getKind() == "Group"
    assert aFeature1.name() == "Color_1"

    aSelectionList = aFeature1.selectionList("group_list")
    assert aSelectionList.size() == 1

    aFeature1 = aCompositeFeature.subFeature(1, False)
    assert aFeature1.getKind() == "Group"
    assert aFeature1.name() == "Color_2"

    aSelectionList = aFeature1.selectionList("group_list")
    assert aSelectionList.size() == 1

    aFeature1 = aCompositeFeature.subFeature(2, False)
    assert aFeature1.getKind() == "Group"
    assert aFeature1.name() == "Material_black"

    aSelectionList = aFeature1.selectionList("group_list")
    assert aSelectionList.size() == 1


    aFeature1 = aCompositeFeature.subFeature(3, False)
    assert aFeature1.getKind() == "Group"
    assert aFeature1.name() == "Material_white"

    aSelectionList = aFeature1.selectionList("group_list")
    assert aSelectionList.size() == 1

    model.end()

if __name__ == '__main__':
    with TemporaryDirectory() as tmp_dir:
        testImportSTEP()

# Copyright (C) 2022-2024  CEA, EDF, OPEN CASCADE
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
      TestImportXAOMem.py
      Unit test of ExchangePlugin_ImportFeature class
"""

#=========================================================================
# Initialization of the test
#=========================================================================

import salome
salome.standalone()
salome.salome_init(embedded=True)

import os
import math
import tempfile

from ModelAPI import *
from GeomAPI import *
from GeomAlgoAPI import *
from salome.shaper import model

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

#=========================================================================
# test Import XAO from memory buffer (bytes array) 
#=========================================================================
def testImportXAOMem():

    # Export from GEOM
    Box_1 = geompy.MakeBoxDXDYDZ(10, 10, 10)
    Group_1 = geompy.CreateGroup(Box_1, geompy.ShapeType["FACE"])
    geompy.UnionIDs(Group_1, [33])
    geompy.addToStudy( Box_1, 'Box10x10x10' )
    geompy.addToStudyInFather( Box_1, Group_1, 'GroupTopFace' )

    Export_buff = geompy.ExportXAOMem(Box_1, [Group_1], [], "XAO")

    # check buffer length
    # export to XAO file and compare size of file and size of buffer
    with tempfile.TemporaryDirectory() as tmpdirname:
        tmpfilename = os.path.join(tmpdirname, "Box.xao")
        exported = geompy.ExportXAO(Box_1, [Group_1], [], "XAO", tmpfilename, "")
        file_stats = os.stat(tmpfilename)
        assert(len(Export_buff) == file_stats.st_size)
        pass

    # Import to SHAPER
    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()

    Import_1 = model.addImportXAOMem(Part_1_doc, Export_buff)
    model.end()

    # Check result
    assert(Import_1.feature().error() == "")
    model.testNbResults(Import_1, 1)
    model.testNbSubResults(Import_1, [0])
    model.testNbSubShapes(Import_1, GeomAPI_Shape.SOLID, [1])
    model.testNbSubShapes(Import_1, GeomAPI_Shape.FACE, [6])
    model.testNbSubShapes(Import_1, GeomAPI_Shape.EDGE, [24])
    model.testNbSubShapes(Import_1, GeomAPI_Shape.VERTEX, [48])
    model.testResultsVolumes(Import_1, [1000])
    model.testResultsAreas(Import_1, [600])

    assert(Import_1.name() == "Box10x10x10")
    assert(Import_1.result().name() == "Box10x10x10_1")
    assert(Import_1.subFeature(0).name() == "GroupTopFace")
    assert(Import_1.subFeature(0).result().name() == "GroupTopFace")

    # check group
    assert(Part_1_doc.size("Groups") == 1)
    res1 = objectToResult(Part_1_doc.object("Groups", 0))
    assert(res1 is not None)
    res1It = GeomAPI_ShapeExplorer(res1.shape(), GeomAPI_Shape.FACE)
    assert(res1It.more())
    shape1 = res1It.current()
    res1It.next()
    assert(not res1It.more())
    p1 = res1.shape().middlePoint()
    aTol = 1.e-7
    assert(math.fabs(p1.x() - 5) <= aTol and math.fabs(p1.y() - 5) <= aTol and math.fabs(p1.z() - 10) <= aTol), "({}, {}, {}) != ({}, {}, {})".format(p1.x(), p1.y(), p1.z(), 5, 5, 10)
    pass

def testImportXAOMem_EmptyName():

    # Export from GEOM
    Box_1 = geompy.MakeBoxDXDYDZ(10, 10, 10)
    Group_1 = geompy.CreateGroup(Box_1, geompy.ShapeType["FACE"])
    geompy.UnionIDs(Group_1, [33])
    # here we do not add box to study to check special case of empty shape name in XAO

    Export_buff = geompy.ExportXAOMem(Box_1, [Group_1], [], "XAO")

    # Import to SHAPER
    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()

    Import_1 = model.addImportXAOMem(Part_1_doc, Export_buff)
    model.end()

    # Check result
    assert(Import_1.feature().error() == "")
    model.testNbResults(Import_1, 1)

    assert(Import_1.name() == "ImportXAOMem")
    assert(Import_1.result().name() == "ImportXAOMem_1")
    assert(Import_1.subFeature(0).name() == "Group_1")
    assert(Import_1.subFeature(0).result().name() == "Group_1")

if __name__ == '__main__':
    #=========================================================================
    # Import a shape from XAO memory buffer
    #=========================================================================
    testImportXAOMem()
    testImportXAOMem_EmptyName()
    #=========================================================================
    # End of test
    #=========================================================================

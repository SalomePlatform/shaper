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
      TestExportXAOMem.py
      Unit test of ExchangePlugin_ExportFeature class
"""

#=========================================================================
# Initialization of the test
#=========================================================================

import salome
salome.standalone()
salome.salome_init(1)

import os
import math
import tempfile

from ModelAPI import *
from GeomAlgoAPI import *
from salome.shaper import model

import GEOM
from salome.geom import geomBuilder
geompy = geomBuilder.New()

#=========================================================================
# test Export XAO to memory buffer (bytes array) 
#=========================================================================
def testExportXAOMem():

    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
    Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Box_1_1/Top")])
    Box_2 = model.addBox(Part_1_doc, 20, 20, 20)
    model.do()

    # Export to memory buffer (bytes array)
    Export_buff1 = model.exportToXAOMem(Part_1_doc, model.selection("SOLID", "Box_1_1"), 'XAO')
    Export_buff2 = model.exportToXAOMem(Part_1_doc, model.selection("SOLID", "Box_2_1"), 'XAO')
    model.end()

    # check buffer length
    # export to XAO file and compare size of file and size of buffer
    #assert(len(Export_buff1) == 4392)
    #assert(len(Export_buff2) == 4287)
    with tempfile.TemporaryDirectory() as tmpdirname:
        tmpfilename = os.path.join(tmpdirname, "Box.xao")
        Export_1 = model.exportToXAO(Part_1_doc, tmpfilename, model.selection("SOLID", "Box_1_1"), 'XAO')
        file_stats = os.stat(tmpfilename)
        assert(len(Export_buff1) == file_stats.st_size)

        Export_2 = model.exportToXAO(Part_1_doc, tmpfilename, model.selection("SOLID", "Box_2_1"), 'XAO')
        file_stats = os.stat(tmpfilename)
        assert(len(Export_buff2) == file_stats.st_size)
        pass

    # Import to GEOM
    (imported1, b1, [], [Group_1], []) = geompy.ImportXAOMem(Export_buff1, theName="Box1")
    (imported2, b2, [], []       , []) = geompy.ImportXAOMem(Export_buff2, theName="Box2")

    # Check result 1
    aTol = 1.e-7
    Props = geompy.BasicProperties(b1)
    # surface area
    aSurface = 600
    assert (math.fabs(Props[1] - aSurface) < aTol), "The surface is wrong: expected = {0}, real = {1}".format(aSurface, Props[1])

    Props = geompy.BasicProperties(Group_1)
    # surface area
    aSurface = 100
    assert (math.fabs(Props[1] - aSurface) < aTol), "The surface is wrong: expected = {0}, real = {1}".format(aSurface, Props[1])

    # Check result 2
    Props = geompy.BasicProperties(b2)
    # surface area
    aSurface = 2400
    assert (math.fabs(Props[1] - aSurface) < aTol), "The surface is wrong: expected = {0}, real = {1}".format(aSurface, Props[1])

if __name__ == '__main__':
    #=========================================================================
    # Export a shape into XAO memory buffer
    #=========================================================================
    testExportXAOMem()
    #=========================================================================
    # End of test
    #=========================================================================

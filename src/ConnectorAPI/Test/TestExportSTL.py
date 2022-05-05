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
      TestExport.py
      Unit test of ExchangePlugin_TestExport class
"""
#=========================================================================
# Initialization of the test
#=========================================================================

import salome
salome.standalone()

import os
import math
from tempfile import TemporaryDirectory

import GEOM

from ModelAPI import *

from salome.shaper import model

from salome.geom import geomBuilder

from GeomAlgoAPI import *

__updated__ = "2015-05-22"

salome.salome_init(1)
geompy = geomBuilder.New()

#=========================================================================
# Help functions
#=========================================================================
def removeFile(theFileName):
    try: os.remove(theFileName)
    except OSError: pass
    assert not os.path.exists(theFileName), \
            "Can not remove file {0}".format(theFileName)

#=========================================================================
# test Export STL 
#=========================================================================
def testExportSTL(theFile, theDelta, theErrorExpected = False):

    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
    Box_2 = model.addBox(Part_1_doc, 20, 20, 20)
    model.do()

    # First export to GEOM
    model.exportToGEOM(Part_1_doc)
    model.end()

    theSurface = 600

    print("theFile=",theFile)

    # deflection relative 0.0001 et Ascii 
    model.exportToSTL(Part_1_doc, theFile, model.selection("SOLID","Box_1_1"),0.0001, 0.5, True,False)

 #==   assert os.path.exists(theFile)
        
    # Check results
    test_stl_1 = geompy.ImportSTL(theFile)
    Props = geompy.BasicProperties(test_stl_1)
    print("\nBasic Properties:")
    print(" Wires length: ", Props[0])
    print(" Surface area: ", Props[1])
    print(" Volume      : ", Props[2])

    aRefSurface = theSurface
    aResSurface = Props[1]
    assert (math.fabs(aResSurface - aRefSurface) < theDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)
    
    removeFile(theFile)

    theSurface =  600
    # deflection relative 0.0001 et binaire
    model.exportToSTL(Part_1_doc, theFile, model.selection("SOLID", "Box_1_1" ),0.0001, 0.5, True,True)

    # Check results
    test_stl_1 = geompy.ImportSTL(theFile)
    Props = geompy.BasicProperties(test_stl_1)
    print("\nBasic Properties:")
    print(" Wires length: ", Props[0])
    print(" Surface area: ", Props[1])
    print(" Volume      : ", Props[2])

    aRefSurface = theSurface
    aResSurface = Props[1]
    assert (math.fabs(aResSurface - aRefSurface) < theDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)
    
    removeFile(theFile)

    theSurface =  600
    # deflection absolue et AScii
    model.exportToSTL(Part_1_doc, theFile, model.selection("SOLID", "Box_1_1" ),0.0001, 0.5, False, False)

    # Check results
    test_stl_1 = geompy.ImportSTL(theFile)
    Props = geompy.BasicProperties(test_stl_1)
    print("\nBasic Properties:")
    print(" Wires length: ", Props[0])
    print(" Surface area: ", Props[1])
    print(" Volume      : ", Props[2])

    aRefSurface = theSurface
    aResSurface = Props[1]
    assert (math.fabs(aResSurface - aRefSurface) < theDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)

    theSurface =  600
    # deflection absolue et binaire
    model.exportToSTL(Part_1_doc, theFile, model.selection("SOLID", "Box_1_1" ),0.0001, 0.5, False,True)

    # Check results
    test_stl_1 = geompy.ImportSTL(theFile)
    Props = geompy.BasicProperties(test_stl_1)
    print("\nBasic Properties:")
    print(" Wires length: ", Props[0])
    print(" Surface area: ", Props[1])
    print(" Volume      : ", Props[2])

    aRefSurface = theSurface
    aResSurface = Props[1]
    assert (math.fabs(aResSurface - aRefSurface) < theDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)

    model.end()

if __name__ == '__main__':
    with TemporaryDirectory() as tmp_dir:
        aRealSurface = 0.00192878
        #=========================================================================
        # Export a shape into STL
        #=========================================================================
        testExportSTL(os.path.join(tmp_dir, "export.stl"), 10 ** -5, True)
        #=========================================================================
        # End of test
        #=========================================================================

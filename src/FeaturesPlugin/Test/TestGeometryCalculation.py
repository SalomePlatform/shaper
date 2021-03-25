# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
      Unit test of ...
"""
#=========================================================================
# Initialization of the test
#=========================================================================

import salome

import os
import math

from ModelAPI import *
from salome.shaper import model



__updated__ = "2020-11-12"


#=========================================================================
# test Geometry calculation
#=========================================================================
def test_Geometry_Calculation():

    model.begin()
    file_path = os.path.join(os.getenv("DATA_DIR"),"Shapes","Brep","box1.brep")
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    Import_1 = model.addImport(Part_1_doc,file_path)
    model.do()

    myDelta = 1e-6
    Props = model.getGeometryCalculation(Part_1_doc,model.selection("SOLID", "box1_1"))

    print(" Geometry calculation:")
    print(" Wires length: ", Props[0])
    print(" Surface area: ", Props[1])
    print(" Volume      : ", Props[2])

    aReflength = 2400
    aReslength = Props[0]
    assert (math.fabs(aReslength - aReflength) < myDelta), "The surface is wrong: expected = {0}, real = {1}".format(aReflength, aReslength)

    aRefSurface = 240000
    aResSurface = Props[1]
    assert (math.fabs(aResSurface - aRefSurface) < myDelta), "The surface is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)

    aRefVolume = 8000000
    aResVolume = Props[2]
    assert (math.fabs(aResVolume - aRefVolume) < myDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefVolume, aResVolume)


if __name__ == '__main__':

    test_Geometry_Calculation()

    #=========================================================================
    # End of test
    #=========================================================================

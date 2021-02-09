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
      Unit test of ...
"""
#=========================================================================
# Initialization of the test
#=========================================================================


import os
import math

from ModelAPI import *
from salome.shaper import model


__updated__ = "2020-11-12"


#=========================================================================
# test Bounding Box  
#=========================================================================
def test_Bounding_Box():

    model.begin()
    partSet = model.moduleDocument()
    Part_1 = model.addPart(partSet)
    Part_1_doc = Part_1.document()
    ### Create Cone
    Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)

    model.do()
    ### Create BoundingBox
    BoundingBox_1 = model.getBoundingBox(Part_1_doc, model.selection("SOLID", "Cone_1_1"))
    model.end()

    myDelta = 1e-6
    Props = model.getGeometryCalculation(Part_1_doc,model.selection("SOLID", "BoundingBox_1_1"))

    print(" Basic Properties:")
    print(" Wires length: ", Props[0])
    print(" Surface area: ", Props[1])
    print(" Volume      : ", Props[2]) 
    
    aReflength = 200
    aReslength = Props[0]
    assert (math.fabs(aReslength - aReflength) < myDelta), "The surface is wrong: expected = {0}, real = {1}".format(aReflength, aReslength)

    aRefSurface = 1600
    aResSurface = Props[1]
    assert (math.fabs(aResSurface - aRefSurface) < myDelta), "The surface is wrong: expected = {0}, real = {1}".format(aRefSurface, aResSurface)

    aRefVolume = 4000
    aResVolume = Props[2]
    assert (math.fabs(aResVolume - aRefVolume) < myDelta), "The volume is wrong: expected = {0}, real = {1}".format(aRefVolume, aResVolume)

    
if __name__ == '__main__':

    test_Bounding_Box()
        
    #=========================================================================
    # End of test
    #=========================================================================

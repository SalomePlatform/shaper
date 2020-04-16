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
      Test1064.py
      Unit test for testing the Part sub-shapes naming, described in the issue 1064

"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2015-10-16"

aSession = ModelAPI_Session.get()

#=========================================================================
# Create a sketch triangle in PartSet
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPartSet.addFeature("Sketch"))
aXOYPlane = objectToResult(aPartSet.objectByName("Construction", "XOY"))
aSketchFeature.selection("External").setValue(aXOYPlane, None)
aFeatureData = aSketchFeature.data()
anArray = aFeatureData.addAttribute("IntArray_1", "IntArray")
aFeatureData.intArray("IntArray_1").setSize(5)
aSession.finishOperation()

assert(aFeatureData.intArray("IntArray_1").size() == 5)

#=========================================================================
# End of test
#=========================================================================

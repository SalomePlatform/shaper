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

#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

import math

aSession = ModelAPI_Session.get()

#=========================================================================
# Test double array attribute
#=========================================================================
aPartSet = aSession.moduleDocument()
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aPartSet.addFeature("Sketch"))
aFeatureData = aSketchFeature.data()
anArray = aFeatureData.addAttribute("double_array", "DoubleArray")
aFeatureData.realArray("double_array").setSize(5)
assert(aFeatureData.realArray("double_array").size() == 5)
aFeatureData.realArray("double_array").setValue(0, 1)
assert(math.fabs(aFeatureData.realArray("double_array").value(0) - 1) < 10 ** -7)
aFeatureData.realArray("double_array").setValue(1, 1.5)
assert(math.fabs(aFeatureData.realArray("double_array").value(1) - 1.5) < 10 ** -7)

#=========================================================================
# End of test
#=========================================================================

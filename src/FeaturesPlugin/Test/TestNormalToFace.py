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
# Initialization of the test
#=========================================================================
model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()

#=========================================================================
# Test 1. Create a normal to a face with only a face
#=========================================================================
Normal_1 = model.getNormal(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
model.do()

#=========================================================================
# Test 2. Create a normal to a face with a face and a vertex
#=========================================================================
Normal_2 = model.getNormal(Part_1_doc, model.selection("FACE", "Box_1_1/Front"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"))
model.do()

model.end()

assert (len(Normal_1.results()) > 0)
assert(Normal_1.feature().error() == "")
anAxisResult = modelAPI_ResultConstruction(Normal_1.feature().firstResult())
assert (anAxisResult is not None)

assert (len(Normal_2.results()) > 0)
assert(Normal_2.feature().error() == "")
anAxisResult = modelAPI_ResultConstruction(Normal_2.feature().firstResult())
assert (anAxisResult is not None)

#=========================================================================
# End of test
#=========================================================================

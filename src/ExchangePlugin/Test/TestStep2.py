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

from salome.shaper import model

import os

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
### Import from step
data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")
#data_dir = os.path.join(os.path.dirname(os.path.abspath("C:\\NewGEOM\\SALOME-9.8.0D\\MODULES\\SRC\\SHAPER_SRC\\src\\ExchangePlugin\\Test\\TestStep2.py")), "data")
Import_1 = model.addImportSTEP(Part_1_doc, os.path.join(data_dir, "pipe_with_colors_and_names.stp"), True, True, True)

model.end()

def checkColor(body, rComponent, gComponent, bComponent):
  assert(body.data().intArray("Color").size() == 3)
  assert(body.data().intArray("Color").value(0) == rComponent)
  assert(body.data().intArray("Color").value(1) == gComponent)
  assert(body.data().intArray("Color").value(2) == bComponent)

# Check the imported names and colors correspond to the exported ones
res = Import_1.result()
assert(res.name() == "pipe_with_colors_and_names_1") # the higher level compound name equals to the file name
assert(res.subResult(0).name() == "2WCL-2.0-316-7-01")
assert(res.subResult(1).name() == "2WCL-2.0-316-7-01__1")
assert(res.subResult(2).name() == "2WCL-2.0-316-7-01__2")
assert(res.subResult(3).name() == "B7WWW-2.0-304-7-01")
assert(res.subResult(4).name() == "B7WWW-2.0-304-7-01__1")
assert(res.subResult(5).name() == "2WCL-2.0-316-7-01__3")
assert(res.subResult(6).name() == "RNT-304-2.000-0.065-WL-7_oa_0")
assert(res.subResult(7).name() == "RNT-304-2.000-0.065-WL-7_oa_1")
assert(res.subResult(8).name() == "RNT-304-2.000-0.065-WL-7_oa_2")
assert(res.subResult(9).name() == "RNT-304-2.000-0.065-WL-7_oa_3")
assert(res.subResult(10).name() == "RNT-304-2.000-0.065-WL-7_oa_4")
rootBody = model.modelAPI_ResultBody(Import_1.feature().firstResult())
checkColor(rootBody.subResult(0), 153, 33, 0)
checkColor(rootBody.subResult(1), 153, 33, 0)
checkColor(rootBody.subResult(2), 153, 33, 0)
checkColor(rootBody.subResult(3), 153, 33, 0)
checkColor(rootBody.subResult(4), 153, 33, 0)
checkColor(rootBody.subResult(5), 153, 33, 0)
checkColor(rootBody.subResult(6), 0, 12, 132)
checkColor(rootBody.subResult(7), 0, 12, 132)
checkColor(rootBody.subResult(8), 0, 12, 132)
checkColor(rootBody.subResult(9), 0, 12, 132)
checkColor(rootBody.subResult(10), 0, 12, 132)

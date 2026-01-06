# Copyright (C) 2014-2026  CEA, EDF
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
      Unit test of makeVertexInsideFace with a cone
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 50, 100, 50)

# Make a cloud of given number of points in range 1-30 on the cone face
for i in range(1, 31):
    model.do()
    PC_1 = model.makeVertexInsideFace(Part_1_doc, model.selection("FACE", "Cone_1_1/Face_1"), i)

    ### Check result
    err = PC_1.feature().error()
    if err != "":
        print(err)
        # this test should not fail with old OCCT
        assert("Improper OCCT version" in err)
        break
    else:
        assert(model.checkPythonDump())

model.end()

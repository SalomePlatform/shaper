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


# Create document
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# =============================================================================
# Prepare all input shapes
# =============================================================================
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], axis = model.selection("EDGE", "PartSet/OX"), distance = 10, keepSubResults = True)
Copy_1_objects = [model.selection("FACE", "Box_1_1/Top"),
                  model.selection("FACE", "Translation_1_1/MF:Translated&Box_2_1/Top"),
                  model.selection("FACE", "Translation_1_1/MF:Translated&Box_2_1/Left")]
Copy_1 = model.addCopy(Part_1_doc, Copy_1_objects, 1)
Copy_1.result().setName("Box_1_1_1")
Copy_1.results()[1].setName("Translation_1_1_1")
Copy_1.results()[2].setName("Translation_1_1_2")
model.do()

# =============================================================================
# Test 1. Glue faces of 2 adjacent solids
# =============================================================================
GlueFaces_1 = model.addGlueFaces(Part_1_doc, [model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Box_1_1")], 1e-07, True)
model.do()

# gluing successful
model.testCompSolid(GlueFaces_1, [2], [2], [11], [20], [12])

# =============================================================================
# Test 2. Glue faces for 2 faces with 1 common edge only
# =============================================================================
GlueFaces_2 = model.addGlueFaces(Part_1_doc, [model.selection("FACE", "Box_1_1_1"), model.selection("FACE", "Translation_1_1_1")], 1e-07, True)
GlueFaces_2.result().setName("GlueFaces_2_1")
GlueFaces_2.result().subResult(0).setName("GlueFaces_2_1_1")
GlueFaces_2.result().subResult(1).setName("GlueFaces_2_1_2")
model.do()

# gluing successful
model.testCompound(GlueFaces_2, [2], [0], [2], [7], [6])

# =============================================================================
# Test 3. Glue faces for 1 solid and 1 faces with 1 common edge
# =============================================================================
Recover_1 = model.addRecover(Part_1_doc, GlueFaces_1, [Box_1.result()])
GlueFaces_3 = model.addGlueFaces(Part_1_doc, [model.selection("FACE", "Translation_1_1_2"), model.selection("SOLID", "Recover_1_1")], 1e-07, True)
GlueFaces_3.result().setName("GlueFaces_3_1")
GlueFaces_3.result().subResult(0).setName("GlueFaces_3_1_1")
GlueFaces_3.result().subResult(1).setName("GlueFaces_3_1_2")
model.end()

# no faces glued
model.testCompound(GlueFaces_3, [2], [1], [7], [15], [10])

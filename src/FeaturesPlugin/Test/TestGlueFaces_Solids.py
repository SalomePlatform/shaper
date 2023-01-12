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
Param_dx = model.addParameter(Part_1_doc, "dx", '10')
Param_alfa = model.addParameter(Part_1_doc, "alfa", '0')

# =============================================================================
# Test 1. Glue faces of 2 adjacent boxes
# =============================================================================
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], axis = model.selection("EDGE", "PartSet/OX"), distance = "dx", keepSubResults = True)
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], axis = model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"), angle = "alfa", keepSubResults = True)
model.do()

GlueFaces_1 = model.addGlueFaces(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Rotation_1_1")], 1.0e-7, True)
model.end()

# gluing successful
model.testCompound(GlueFaces_1, [2], [2], [11], [20], [12])

# =============================================================================
# Test 2. Glue faces for 2 solids with 1 common edge only
# =============================================================================
model.undo() # to reuse Box_1_1, Translation_1_1, and Rotation_1_1
model.begin()
Param_alfa.setValue(90)
model.do()

GlueFaces_2 = model.addGlueFaces(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Rotation_1_1")], 1.0e-7, True)
model.end()

# gluing successful
model.testCompound(GlueFaces_2, [2], [2], [12], [23], [14])

# =============================================================================
# Test 3. Glue faces for 2 solids with 2 adjacent faces above default tolerance
# =============================================================================
model.undo() # to reuse Box_1_1, Translation_1_1, and Rotation_1_1
model.begin()

Param_alfa.setValue(0)
Param_dx.setValue(10.0001)
model.do()

GlueFaces_3 = model.addGlueFaces(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Rotation_1_1")], 1.0e-7, True)
model.end()

# no faces glued
model.testCompound(GlueFaces_3, [2], [2], [12], [24], [16])

# =============================================================================
# Test 4. Glue faces for 2 solids with 2 adjacent faces using different tolerance
# =============================================================================
model.undo() # to reuse Box_1_1, Translation_1_1, and Rotation_1_1
model.begin()

tol = 1.e-4
GlueFaces_4 = model.addGlueFaces(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Rotation_1_1")], tol, True)
model.end()

# gluing successful
model.testCompound(GlueFaces_4, [2], [2], [11], [20], [12])

# =============================================================================
# Test 5. Check Python dump
# =============================================================================
assert(model.checkPythonDump())

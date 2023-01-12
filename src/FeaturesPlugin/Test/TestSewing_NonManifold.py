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
Param_alfa = model.addParameter(Part_1_doc, "alfa", '90')

# =============================================================================
# Test 1. Sew two shells with a single common edge
# =============================================================================
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Vertex_1 = model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]")
Vertex_2 = model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]")
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], startPoint = Vertex_1, endPoint = Vertex_2, keepSubResults = True)

Shell_1_objects = [model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Box_1_1/Front")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)

Shell_2_objects = [model.selection("FACE", "Translation_1_1/MF:Translated&Box_2_1/Back"), model.selection("FACE", "Translation_1_1/MF:Translated&Box_2_1/Bottom")]
Shell_2 = model.addShell(Part_1_doc, Shell_2_objects)
model.do()

Sewing_1 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-07, allowNonManifold = False, alwaysCreateResult = True)
model.end()

# sewing failed (result is a compound with the two input shells)
model.testResults(Sewing_1, 1, [2], [2], [4], [14], [12])

# =============================================================================
# Test 2. Sew the same two shells allowing non-manifold results
# =============================================================================
model.undo()
model.begin()

Sewing_2 = model.addSewing(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_1_1")], 1e-07, allowNonManifold = True, alwaysCreateResult = True)
model.end()

# sewing successful
model.testResults(Sewing_2, 1, [0], [1], [4], [13], [10])

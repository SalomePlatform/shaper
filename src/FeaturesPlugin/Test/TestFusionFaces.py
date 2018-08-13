## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

# Create document
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# =============================================================================
# Test 1. Fusion faces for shell of 2 adjacent faces lying on plane
# =============================================================================
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OX"), 10)

Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Translation_1_1/Translated_Face_1")])
model.do()

model.checkResult(Shell_1, model, 1, [0], [0], [2], [8], [16])

FusionFaces_1 = model.addFusionFaces(Part_1_doc, model.selection("SHELL", "Shell_1_1"))
model.do()

model.checkResult(FusionFaces_1, model, 1, [0], [0], [1], [4], [8])

# =============================================================================
# Test 2. Fusion faces for solid of 2 adjacent boxes
# =============================================================================
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Translation_1_1")], [])
model.do()

model.checkResult(Fuse_1, model, 1, [0], [1], [10], [40], [80])

FusionFaces_2 = model.addFusionFaces(Part_1_doc, model.selection("SOLID", "Fuse_1_1"))
model.do()

model.checkResult(FusionFaces_2, model, 1, [0], [1], [6], [24], [48])

# =============================================================================
# Test 3. Fusion faces for shell of adjacent faces lying on cylindrical surface
# =============================================================================
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_1")])
Point_1 = model.addPoint(Part_1_doc, 0, 0, 5)
Plane_1 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OZ"), model.selection("VERTEX", "Point_1"), True)

Partition_1 = model.addPartition(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("FACE", "Plane_1")])
model.do()

model.checkResult(Partition_1, model, 1, [0], [0], [2], [8], [16])

FusionFaces_3 = model.addFusionFaces(Part_1_doc, model.selection("SHELL", "Partition_1_1"))
model.do()

model.checkResult(FusionFaces_3, model, 1, [0], [0], [1], [4], [8])

# =============================================================================
# Test 4. Check subshapes naming
# =============================================================================
#model.testHaveNamingSubshapes(FusionFaces_1, model, Part_1_doc)
#model.testHaveNamingSubshapes(FusionFaces_2, model, Part_1_doc)
#model.testHaveNamingSubshapes(FusionFaces_3, model, Part_1_doc)
model.end()

# =============================================================================
# Test 5. Check Python dump
# =============================================================================
assert(model.checkPythonDump())

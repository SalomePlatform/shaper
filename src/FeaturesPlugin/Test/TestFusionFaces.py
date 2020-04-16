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

model.do()
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Box_1_1/Top"), model.selection("FACE", "Translation_1_1/MF:Translated&Box_2_1/Top")])
model.do()

model.checkResult(Shell_1, model, 1, [0], [0], [2], [8], [16])

FusionFaces_1 = model.addFusionFaces(Part_1_doc, model.selection("SHELL", "Shell_1_1"))
model.end()

model.checkResult(FusionFaces_1, model, 1, [0], [0], [1], [4], [8])

model.undo() # to reuse Box_1_1 and Translation_1_1
model.undo()
model.begin()

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

Partition_1 = model.addPartition(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("FACE", "Plane_1")])
model.do()

model.checkResult(Partition_1, model, 1, [0], [0], [2], [8], [16])

FusionFaces_3 = model.addFusionFaces(Part_1_doc, model.selection("SHELL", "Partition_1_1"))
model.do()

model.checkResult(FusionFaces_3, model, 1, [0], [0], [1], [4], [8])

# =============================================================================
# Test 4. Fusion faces for compsolid
# =============================================================================
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(25, 40, -50, 40)
SketchLine_2 = Sketch_1.addLine(-50, 40, -50, 0)
SketchLine_3 = Sketch_1.addLine(-50, 0, 25, 0)
SketchLine_4 = Sketch_1.addLine(25, 0, 25, 40)
SketchLine_5 = Sketch_1.addLine(-20, 40, -20, 0)
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Box_3 = model.addBox(Part_1_doc, 10, 10, 10)

Fuse_2 = model.addFuse(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("SOLID", "Box_3_1")])
model.do()

model.checkResult(Fuse_2, model, 1, [1], [1], [14], [64], [128])

FusionFaces_4 = model.addFusionFaces(Part_1_doc, model.selection("COMPSOLID", "Fuse_2_1"))
model.do()

model.checkResult(FusionFaces_4, model, 1, [1], [1], [6], [24], [48])

# =============================================================================
# Test 5. Fusion faces for compound
# =============================================================================
Box_4 = model.addBox(Part_1_doc, 10, 10, 10)
Box_5 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_5_1")], 0, 0, 5)
Box_6 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_3 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_6_1")], 20, 0, 0)
Box_7 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_4 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_7_1")], 20, 0, 5)
Fuse_3 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Box_4_1"), model.selection("SOLID", "Translation_2_1")], [])
Fuse_4 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Translation_3_1"), model.selection("SOLID", "Translation_4_1")], [])

Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Fuse_3_1"), model.selection("SOLID", "Fuse_4_1")])
model.do()

model.checkResult(Compound_1, model, 1, [2], [2], [28], [112], [224])

FusionFaces_5 = model.addFusionFaces(Part_1_doc, model.selection("COMPOUND", "Compound_1_1"))
model.do()

model.checkResult(FusionFaces_5, model, 1, [2], [2], [12], [48], [96])

# =============================================================================
# Test 6. Check subshapes naming
# =============================================================================
#model.testHaveNamingSubshapes(FusionFaces_1, model, Part_1_doc)
model.testHaveNamingSubshapes(FusionFaces_2, model, Part_1_doc)
#model.testHaveNamingSubshapes(FusionFaces_3, model, Part_1_doc)
model.testHaveNamingSubshapes(FusionFaces_4, model, Part_1_doc)
model.testHaveNamingSubshapes(FusionFaces_5, model, Part_1_doc)
model.end()

# =============================================================================
# Test 7. Check Python dump
# =============================================================================
assert(model.checkPythonDump())

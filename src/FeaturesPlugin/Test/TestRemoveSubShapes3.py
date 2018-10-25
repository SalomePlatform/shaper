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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

# =============================================================================
# Test 1. Remove face from shell
# =============================================================================
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_1")])
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_1")])
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OZ"), 10)
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Translation_1_1"), model.selection("FACE", "Face_1_1")])

Remove_SubShapes_1 = model.addRemoveSubShapes(Part_1_doc, model.selection("SHELL", "Shell_1_1"))
Remove_SubShapes_1.setSubShapesToRemove([model.selection("FACE", "Shell_1_1/Modified_Face&Face_2_1/Face_2_1")])
model.do()

model.checkResult(Remove_SubShapes_1, model, 1, [0], [0], [1], [4], [8])
model.testHaveNamingSubshapes(Remove_SubShapes_1, model, Part_1_doc)

# =============================================================================
# Test 2. Remove faces from compound
# =============================================================================
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()

Cylinder_1 = model.addCylinder(Part_2_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Face_1 = model.addFace(Part_2_doc, [model.selection("FACE", "Cylinder_1_1/Face_1")])
LinearCopy_1 = model.addMultiTranslation(Part_2_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OZ"), 10, 2)
LinearCopy_2 = model.addMultiTranslation(Part_2_doc, [model.selection("COMPOUND", "LinearCopy_1_1")], model.selection("EDGE", "PartSet/OZ"), 20, 2)

Remove_SubShapes_2 = model.addRemoveSubShapes(Part_2_doc, model.selection("COMPOUND", "LinearCopy_2_1"))
Remove_SubShapes_2.setSubShapesToRemove([model.selection("FACE", "LinearCopy_2_1_2_2"), model.selection("FACE", "LinearCopy_2_1_2_1")])
model.do()

model.checkResult(Remove_SubShapes_2, model, 1, [2], [0], [2], [8], [16])
model.testHaveNamingSubshapes(Remove_SubShapes_2, model, Part_2_doc)

# =============================================================================
# Test 3. Remove solid from compsolid
# =============================================================================
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()

Box_1 = model.addBox(Part_3_doc, 10, 10, 10)
LinearCopy_1 = model.addMultiTranslation(Part_3_doc, [model.selection("SOLID", "Box_1_1")], model.selection("EDGE", "PartSet/OY"), 10, 6)
CompSolid_1_objects = [model.selection("SOLID", "LinearCopy_1_1_1"), model.selection("SOLID", "LinearCopy_1_1_2"), model.selection("SOLID", "LinearCopy_1_1_3"), model.selection("SOLID", "LinearCopy_1_1_4"), model.selection("SOLID", "LinearCopy_1_1_5"), model.selection("SOLID", "LinearCopy_1_1_6")]
CompSolid_1 = model.addCompSolid(Part_3_doc, CompSolid_1_objects)

Remove_SubShapes_3_objects = [model.selection("SOLID", "CompSolid_1_1_3"), model.selection("SOLID", "CompSolid_1_1_4")]
Remove_SubShapes_3 = model.addRemoveSubShapes(Part_3_doc, model.selection("COMPSOLID", "CompSolid_1_1"))
Remove_SubShapes_3.setSubShapesToRemove(Remove_SubShapes_3_objects)
model.do()

model.checkResult(Remove_SubShapes_3, model, 1, [4], [4], [24], [96], [192])
model.testHaveNamingSubshapes(Remove_SubShapes_3, model, Part_3_doc)

# =============================================================================
# Test 4. Check Python dump
# =============================================================================
model.end()
assert(model.checkPythonDump())

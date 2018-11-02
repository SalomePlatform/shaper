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

# Create sketch
#
#  2         4
#
#       3
#
#  1         5
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchPoint_1 = Sketch_1.addPoint(0, 0)
SketchPoint_2 = Sketch_1.addPoint(0, 50)
SketchPoint_3 = Sketch_1.addPoint(25, 25)
SketchPoint_4 = Sketch_1.addPoint(50, 50)
SketchPoint_5 = Sketch_1.addPoint(50, 0)

SketchLine_1 = Sketch_1.addLine(0, 0, -45, -45)
SketchLine_2 = Sketch_1.addLine(0, 0, 45, 45)
model.do()

# Get sketch points
base_name = "Sketch_1/SketchPoint_"
p_1, p_2, p_3, p_4, p_5 = [model.selection("VERTEX", base_name + str(i + 1)) for i in range(0, 5)]

# Get sketch edges
Tangent_1 = model.selection("EDGE", "Sketch_1/SketchLine_1")
Tangent_2 = model.selection("EDGE", "Sketch_1/SketchLine_2")

# =============================================================================
# Test 1. Create curve 1-2-3-4-5, closed off, reorder off, without tangents
# =============================================================================
Interpolation_1 = model.addInterpolation(Part_1_doc, [p_1, p_2, p_3, p_4, p_5], False, False)
model.do()

model.checkResult(Interpolation_1, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 2. Create curve 1-2-3-4-5-1, closed on, reorder off, without tangents
# =============================================================================
Interpolation_2 = model.addInterpolation(Part_1_doc, [p_1, p_2, p_3, p_4, p_5], True, False)
model.do()

model.checkResult(Interpolation_2, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 3. Create curve 1-2-3-4, closed off, reorder on, without tangents
# =============================================================================
Interpolation_3 = model.addInterpolation(Part_1_doc, [p_1, p_2, p_3, p_4], False, True)
model.do()

model.checkResult(Interpolation_3, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 4. Create curve 1-2-3-5, closed on, reorder on, without tangents
# =============================================================================
Interpolation_4 = model.addInterpolation(Part_1_doc, [p_1, p_2, p_3, p_5], True, True)
model.do()

model.checkResult(Interpolation_4, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 5. Create curve 1-2-3-4-5, closed off, reorder off, with tangents
# =============================================================================

Interpolation_5 = model.addInterpolation(Part_1_doc, [p_1, p_2, p_3, p_4, p_5],
                                         Tangent_1, Tangent_2, False, False)
model.do()

model.checkResult(Interpolation_5, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 6. Try to create closed curve 1-2-1, closed off, reorder off, without tangents
# =============================================================================
Interpolation_6 = model.addInterpolation(Part_1_doc, [p_1, p_2, p_1], False, False)
model.do()

model.testNbResults(Interpolation_6, 0)

# =============================================================================
# Test 7. Try to create curve on a single point 3
# =============================================================================
Interpolation_7 = model.addInterpolation(Part_1_doc, [p_3], False, False)
model.do()

model.testNbResults(Interpolation_7, 0)

# =============================================================================
# Test 8. Create curve on box vertices, closed off, reorder off, without tangents
# =============================================================================
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Box_1 = model.addBox(Part_2_doc, 10, 10, 10)

point_names = ("[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]",
               "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]",
               "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]",
               "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]",
               "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]",
               "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]",
               "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]",
               "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Bottom]")
points = [model.selection("VERTEX", name) for name in point_names]

Interpolation_8 = model.addInterpolation(Part_2_doc, points, False, False)
model.do()

model.checkResult(Interpolation_8, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 9. Create curve on box vertices, closed off, reorder off, with tangents
# =============================================================================
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Box_1 = model.addBox(Part_3_doc, 20, 30, 40)

points = [model.selection("VERTEX", name) for name in point_names]
Tangent_1 = model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]")
Tangent_2 = model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]")

Interpolation_9 = model.addInterpolation(Part_3_doc, points,
                                         Tangent_1, Tangent_2, False, False)
model.do()

model.checkResult(Interpolation_9, model, 1, [0], [0], [0], [1], [2])

# =============================================================================
# Test 10. Create curve using equal vertices
# =============================================================================
Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()

Point_1 = model.addPoint(Part_4_doc, 0, 0, 0)
Point_2 = model.addPoint(Part_4_doc, 0, 0, 0)
P_1 = model.selection("VERTEX", "Point_1")
P_2 = model.selection("VERTEX", "Point_2")

Interpolation_10 = model.addInterpolation(Part_4_doc, [P_1, P_2],
                                          False, False)
model.do()
model.testNbResults(Interpolation_10, 0)

# =============================================================================
# Test 11. Check subshapes naming
# =============================================================================
model.testHaveNamingSubshapes(Interpolation_5, model, Part_1_doc)
model.testHaveNamingSubshapes(Interpolation_9, model, Part_3_doc)
model.end()

# =============================================================================
# Test 12. Check Python dump
# =============================================================================
assert(model.checkPythonDump())

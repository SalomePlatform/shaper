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
model.do()

# Get sketch points
base_name = "Sketch_1/SketchPoint_"
p_1, p_2, p_3, p_4, p_5 = [model.selection("VERTEX", base_name + str(i + 1)) for i in range(0, 5)]

# =============================================================================
# Test 1. Create unclosed polyline 1-2-3-4-5
# =============================================================================
Polyline_1 = model.addPolyline3D(Part_1_doc, [p_1, p_2, p_3, p_4, p_5], False)
model.do()

model.checkResult(Polyline_1, model, 1, [0], [0], [0], [4], [4*2])

# =============================================================================
# Test 2. Create closed polyline 1-2-3-4-5-1
# =============================================================================
Polyline_2 = model.addPolyline3D(Part_1_doc, [p_1, p_2, p_3, p_4, p_5], True)
model.do()

model.checkResult(Polyline_2, model, 1, [0], [0], [0], [5], [5*2])

# =============================================================================
# Test 3. Try to create self-intersected unclosed polyline 2-5-4-1
# =============================================================================
Polyline_3 = model.addPolyline3D(Part_1_doc, [p_2, p_5, p_4, p_1], False)
model.do()

model.testNbResults(Polyline_3, 0)

# =============================================================================
# Test 4. Try to create self-intersected closed polyline 2-4-1-5-2
# =============================================================================
Polyline_4 = model.addPolyline3D(Part_1_doc, [p_2, p_4, p_1, p_5], True)
model.do()

model.testNbResults(Polyline_4, 0)

# =============================================================================
# Test 5. Try to create closed polyline 1-2-1
# =============================================================================
Polyline_5 = model.addPolyline3D(Part_1_doc, [p_1, p_2], True)
model.do()

model.testNbResults(Polyline_5, 0)

# =============================================================================
# Test 6. Try to create unclosed polyline on a single point 3
# =============================================================================
Polyline_6 = model.addPolyline3D(Part_1_doc, [p_3], False)
model.do()

model.testNbResults(Polyline_6, 0)

# =============================================================================
# Test 7. Create unclosed polyline on box vertices
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

Polyline_7 = model.addPolyline3D(Part_2_doc, points, False)
model.do()

model.checkResult(Polyline_7, model, 1, [0], [0], [0], [7], [7*2])

# =============================================================================
# Test 8. Create closed polyline on box vertices
# =============================================================================
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Box_1 = model.addBox(Part_3_doc, 10, 10, 10)

points = [model.selection("VERTEX", name) for name in point_names]

Polyline_8 = model.addPolyline3D(Part_3_doc, points, True)
model.do()

model.checkResult(Polyline_8, model, 1, [0], [0], [0], [8], [8*2])

# =============================================================================
# Test 9. Create polyline using equal vertices
# =============================================================================
Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()

Point_1 = model.addPoint(Part_4_doc, 0, 0, 0)
Point_2 = model.addPoint(Part_4_doc, 0, 0, 0)
P_1 = model.selection("VERTEX", "Point_1")
P_2 = model.selection("VERTEX", "Point_2")

Polyline_9 = model.addPolyline3D(Part_4_doc, [P_1, P_2], False)
model.do()

model.testNbResults(Polyline_9, 0)

# =============================================================================
# Test 10. Check subshapes naming
# =============================================================================
model.testHaveNamingSubshapes(Polyline_1, model, Part_1_doc)
model.end()

# =============================================================================
# Test 11. Check Python dump
# =============================================================================
assert(model.checkPythonDump())

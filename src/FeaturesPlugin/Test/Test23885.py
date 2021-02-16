# Copyright (C) 2020  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(45, 45, 28)
model.do()

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchCircle
SketchCircle_2 = Sketch_2.addCircle(79, 72, 29.30)
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, 0, "Faces|Wires")

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r")], model.selection(), 10, 0, "Faces|Wires")

### Create Compound
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1")])

### Create ExtrusionCut
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), [model.selection("SOLID", "Compound_1_1_2")])

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Compound_1_1_2/Modified_Face&Extrusion_2_1/To_Face"))

### Create SketchCircle
SketchCircle_3 = Sketch_3.addCircle(88, 78, 6)
ExtrusionCut_1.setNestedSketch(Sketch_3)

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [2])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [36])
model.testResultsVolumes(ExtrusionCut_1, [50469.37182])

assert(model.checkPythonDump())

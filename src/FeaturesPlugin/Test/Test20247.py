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

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1")], vector = [200, 100, 0], keepSubResults = True)

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Translation_1_1/MF:Translated&Box_1_1/Top"), 3, False)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(205, 105, 3)
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("FACE", "Translation_1_1/MF:Translated&Box_1_1/Top"), 0, model.selection(), 0, "Faces|Wires")

model.end()

from GeomAPI import *

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [0])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Extrusion_1, [84.823001647])

assert(model.checkPythonDump())

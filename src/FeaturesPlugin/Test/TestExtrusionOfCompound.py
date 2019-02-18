# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(15, -20, 8)
SketchCircle_2 = Sketch_1.addCircle(35, -5, 15)
model.do()
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")])
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], model.selection(), model.selection(), 0, model.selection("FACE", "PartSet/XOY"), 10)
model.do()

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Sketch_2 = model.addSketch(Part_2_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(15, -20, 8)
SketchCircle_4 = Sketch_2.addCircle(35, -5, 15)
model.do()
Compound_2 = model.addCompound(Part_2_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f")])
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("COMPOUND", "Compound_1_1")], model.selection(), 10, 0)
model.do()

model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Extrusion_1, 1)
model.testNbSubResults(Extrusion_1, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.EDGE, [12])
model.testNbSubShapes(Extrusion_1, GeomAPI_Shape.VERTEX, [24])
model.testResultsVolumes(Extrusion_1, [9079.2027688745])

model.testNbResults(Extrusion_2, 1)
model.testNbSubResults(Extrusion_2, [2])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.EDGE, [12])
model.testNbSubShapes(Extrusion_2, GeomAPI_Shape.VERTEX, [24])
model.testResultsVolumes(Extrusion_2, [9079.2027688745])

assert(model.checkPythonDump())
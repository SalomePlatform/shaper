# Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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
SketchPoint_1 = Sketch_1.addPoint(-61.63122212692967, 34.13774013722127)
SketchPoint_2 = Sketch_1.addPoint(-27.26436963979418, 34.59596483704974)
SketchPoint_3 = Sketch_1.addPoint(25.43147084048027, 34.59596483704974)
SketchPoint_4 = Sketch_1.addPoint(54.29962692967411, 33.67951543739281)
model.do()
Compound_1_objects = [model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("VERTEX", "Sketch_1/SketchPoint_2"), model.selection("VERTEX", "Sketch_1/SketchPoint_3"), model.selection("VERTEX", "Sketch_1/SketchPoint_4")]
Compound_1 = model.addCompound(Part_1_doc, Compound_1_objects)
Compound_2 = model.addCompound(Part_1_doc, [model.selection("VERTEX", "Sketch_1/SketchPoint_2"), model.selection("VERTEX", "Sketch_1/SketchPoint_3")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [2])

assert(model.checkPythonDump())

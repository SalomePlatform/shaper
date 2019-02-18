# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-49.71737993138937, 2.062011149228122, 25.20652382544127)
SketchCircle_2 = Sketch_1.addCircle(41.46933533447686, 0.6873370497427074, 27.03914027119833)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(-15.35052744425385, 1.145561749571189, 23.35148318113706)
SketchCircle_4 = Sketch_2.addCircle(82.70955831903943, -3.436685248713552, 26.06650182177207)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2"), model.selection("EDGE", "Sketch_1/SketchCircle_2_2")])
Face_2 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchCircle_3_2"), model.selection("EDGE", "Sketch_2/SketchCircle_4_2")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("FACE", "Face_2_1"), model.selection("FACE", "Face_2_2")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [8])

assert(model.checkPythonDump())

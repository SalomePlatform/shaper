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
SketchLine_1 = Sketch_1.addLine(-13.70458696967078, 45.46669742770666, 15.88340489230536, 45.46669742770666)
SketchLine_2 = Sketch_1.addLine(-13.21145377197118, 31.16583469441819, 14.89713849690615, 31.41240129326799)
SketchLine_3 = Sketch_1.addLine(-5.074756009927738, 49.65832960815328, -4.828189411077946, 26.48106931627197)
SketchLine_4 = Sketch_1.addLine(7.993273729111718, 49.41176300930348, 8.23984032796152, 26.48106931627197)
model.do()
Compound_1 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_4")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [6])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [12])

assert(model.checkPythonDump())

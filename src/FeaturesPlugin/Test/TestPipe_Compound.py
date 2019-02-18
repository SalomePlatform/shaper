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
SketchLine_1 = Sketch_1.addLine(50, 15, 100, 20)
SketchLine_2 = Sketch_1.addLine(30, 10, 50, 0)
SketchArc_1 = Sketch_1.addArc(30, 40, 60, 40, 30, 70, False)
model.do()
Compound_1 = model.addCompound(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
model.do()
Pipe_1 = model.addPipe(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], model.selection("EDGE", "Sketch_1/SketchArc_1_2"))
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [2])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [16])

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
SketchLine_1 = Sketch_1.addLine(-31.34514549929152, 46.39906406145125, -4.124361249906777, 13.81661018718769)
SketchLine_2 = Sketch_1.addLine(-4.124361249906777, 13.81661018718769, 21.85911462450594, 45.98662793646056)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_3 = Sketch_2.addLine(-23.92129524945932, 7.630068312327553, -4.124361249906777, 34.02598031173091)
SketchLine_4 = Sketch_2.addLine(-4.124361249906777, 34.02598031173091, 14.43526437467373, 6.392759937355498)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
model.do()
Wire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_2/SketchLine_3"), model.selection("EDGE", "Sketch_2/SketchLine_4")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("WIRE", "Wire_1_1"), model.selection("WIRE", "Wire_2_1")])
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [16])

assert(model.checkPythonDump())

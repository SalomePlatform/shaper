# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_1.addCircle(-12.81989708404803, 27.54545454545454, 25.29331046312177)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(22.86792452830187, 26.50600343053173, 30.90704565816075)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("FACE", "Face_2_1")], True)
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [4])

assert(model.checkPythonDump())

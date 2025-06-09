# Copyright (C) 2025  CEA, EDF
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
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(10, 0, 1)
Sketch_1.setCoincident(SketchLine_2.result(), SketchCircle_1.center(), True)
Sketch_1.setRadius(SketchCircle_1.results()[1], 1, True)
model.do()
Sketch_1.setName("base")
Sketch_1.result().setName("base")

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_3 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_2.addProjection(model.selection("VERTEX", "base/SketchCircle_1"), False)
SketchPoint_1 = SketchProjection_4.createdFeature()
SketchArc_1 = Sketch_2.addArc(4, 0, 10, 0, 4, 6, False)
Sketch_2.setCoincident(SketchLine_3.result(), SketchArc_1.center(), True)
Sketch_2.setCoincident(SketchPoint_1.result(), SketchArc_1.startPoint(), True)
Sketch_2.setRadius(SketchArc_1.results()[1], 6, True)
model.do()
Sketch_2.setName("path")
Sketch_2.result().setName("path")

Pipe_1 = model.addPipe(Part_1_doc, [model.selection("FACE", "base/Face-SketchCircle_1_2f")], model.selection("EDGE", "path/SketchArc_1_2"))
model.end()


from GeomAPI import GeomAPI_Shape

model.testNbResults(Pipe_1, 1)
model.testNbSubResults(Pipe_1, [0])
model.testNbUniqueSubShapes(Pipe_1, GeomAPI_Shape.SOLID, [1])
model.testNbUniqueSubShapes(Pipe_1, GeomAPI_Shape.FACE, [3])
model.testNbUniqueSubShapes(Pipe_1, GeomAPI_Shape.EDGE, [3])
model.testNbUniqueSubShapes(Pipe_1, GeomAPI_Shape.VERTEX, [2])
model.testResultsVolumes(Pipe_1, [29.608813203265])


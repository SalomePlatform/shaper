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
SketchCircle_1 = Sketch_1.addCircle(-6.063464837049752, 0.8662092624356817, 32.2304349637617)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchCircle_1_2")])
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_2 = Sketch_2.addCircle(33.45888578231592, 3.635557519489646, 33.65582143213236)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_2.results()[1])
model.do()
Face_2 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("FACE", "Face_1_1")], [model.selection("FACE", "Face_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [4])

assert(model.checkPythonDump())

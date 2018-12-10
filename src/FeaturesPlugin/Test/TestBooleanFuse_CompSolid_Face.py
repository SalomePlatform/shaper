## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(52.27216563767099, 0, 18.66006927176168)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
SketchCircle_2 = Sketch_1.addCircle(79.68333887668101, 0, 24.16077154338278)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_2 = Sketch_2.addLine(73.13136007081269, -14.25937060400964, 2.118687665902372, -14.25937060400964)
SketchLine_3 = Sketch_2.addLine(2.118687665902372, -14.25937060400964, 2.118687665902372, 26.70061277610758)
SketchLine_4 = Sketch_2.addLine(2.118687665902372, 26.70061277610758, 73.13136007081269, 26.70061277610758)
SketchLine_5 = Sketch_2.addLine(73.13136007081269, 26.70061277610758, 73.13136007081269, -14.25937060400964)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_2.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_3.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_5.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r")])
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], [model.selection("FACE", "Face_1_1")])

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [4])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [16])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [32])
model.testResultsVolumes(Fuse_1, [785.398163397447774514148477465])

Fuse_2 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_3"), model.selection("SOLID", "Extrusion_1_1_1")], [model.selection("FACE", "Fuse_1_1_1")])
assert(Fuse_2.feature().error() != "")

model.end()

## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-22.17495711835335, 33.08919382504288, -50.93310463121783, 33.08919382504288)
SketchLine_2 = Sketch_1.addLine(-50.93310463121783, 33.08919382504288, -50.93310463121783, 7.102915951972556)
SketchLine_3 = Sketch_1.addLine(-50.93310463121783, 7.102915951972556, -22.17495711835335, 7.102915951972556)
SketchLine_4 = Sketch_1.addLine(-22.17495711835335, 7.102915951972556, -22.17495711835335, 33.08919382504288)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_5 = Sketch_2.addLine(-38.01624011480907, 11.11021000825241, -18.03083750064881, 11.11021000825241)
SketchLine_6 = Sketch_2.addLine(-18.03083750064881, 11.11021000825241, -18.03083750064881, 22.05701811772883)
SketchLine_7 = Sketch_2.addLine(-18.03083750064881, 22.05701811772883, -38.01624011480907, 22.05701811772883)
SketchLine_8 = Sketch_2.addLine(-38.01624011480907, 22.05701811772883, -38.01624011480907, 11.11021000825241)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Wire-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f")], model.selection(), 30, -20)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_2_1")])
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [11])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [96])

assert(model.checkPythonDump())

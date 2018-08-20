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
SketchLine_1 = Sketch_1.addLine(-8.142167372244474, 30.79863310370742, -42.12686596943888, 30.79863310370742)
SketchLine_1.result().setColor(225, 0, 0)
SketchLine_2 = Sketch_1.addLine(-42.12686596943888, 30.79863310370742, -42.12686596943888, -6.018123709919844)
SketchLine_2.result().setColor(225, 0, 0)
SketchLine_3 = Sketch_1.addLine(-42.12686596943888, -6.018123709919844, -8.142167372244474, -6.018123709919844)
SketchLine_3.result().setColor(225, 0, 0)
SketchLine_4 = Sketch_1.addLine(-8.142167372244474, -6.018123709919844, -8.142167372244474, 30.79863310370742)
SketchLine_4.result().setColor(225, 0, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(49.91502606462924, 30.79863310370742, 15.22231291332662, 30.79863310370742)
SketchLine_5.result().setColor(225, 0, 0)
SketchLine_6 = Sketch_1.addLine(15.22231291332662, 30.79863310370742, 15.22231291332662, -5.310109155811627)
SketchLine_6.result().setColor(225, 0, 0)
SketchLine_7 = Sketch_1.addLine(15.22231291332662, -5.310109155811627, 49.91502606462924, -5.310109155811627)
SketchLine_7.result().setColor(225, 0, 0)
SketchLine_8 = Sketch_1.addLine(49.91502606462924, -5.310109155811627, 49.91502606462924, 30.79863310370742)
SketchLine_8.result().setColor(225, 0, 0)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_8.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 10)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchLine_9 = Sketch_2.addLine(32.99903071986655, 54.47604223124301, -21.74338409862735, 54.47604223124301)
SketchLine_9.result().setColor(225, 0, 0)
SketchLine_10 = Sketch_2.addLine(-21.74338409862735, 54.47604223124301, -21.74338409862735, -10.16156298864041)
SketchLine_10.result().setColor(225, 0, 0)
SketchLine_11 = Sketch_2.addLine(-21.74338409862735, -10.16156298864041, 32.99903071986655, -10.16156298864041)
SketchLine_11.result().setColor(225, 0, 0)
SketchLine_12 = Sketch_2.addLine(32.99903071986655, -10.16156298864041, 32.99903071986655, 54.47604223124301)
SketchLine_12.result().setColor(225, 0, 0)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_6 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_12.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_9f-SketchLine_10f-SketchLine_11f-SketchLine_12f")])
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("SOLID", "Extrusion_1_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("FACE", "Face_1_1"), model.selection("COMPOUND", "Compound_1_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [16])

assert(model.checkPythonDump())

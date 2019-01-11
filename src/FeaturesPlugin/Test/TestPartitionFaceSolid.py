## Copyright (C) 2017  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchLine_1 = Sketch_1.addLine(29.00299588076832, 85.46917598420691, 38.5336063380206, 85.46917598420691)
SketchLine_2 = Sketch_1.addLine(38.5336063380206, 85.46917598420691, 51.24108694769031, 59.52473640613128)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(51.24108694769031, 59.52473640613128, 51.24108694769031, 20.87281621838598)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(51.24108694769031, 20.87281621838598, 74.53813473208469, 20.87281621838598)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(74.53813473208469, 20.87281621838598, 74.53813473208469, 7.635857249980032)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(74.53813473208469, 7.635857249980032, 29.00299588076832, 7.635857249980032)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(29.00299588076832, 7.635857249980032, 29.00299588076832, 85.46917598420691)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_7.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_6.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_1.result())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OZ"), 180, 0)
Sketch_2 = model.addSketch(Part_1_doc,  model.standardPlane("YOZ"))
SketchLine_8 = Sketch_2.addLine(61.72045613220821, 51.92790864086226, -64.56856594464165, 51.92790864086226)
SketchLine_9 = Sketch_2.addLine(-64.56856594464165, 51.92790864086226, -64.56856594464165, -5.323114700643004)
SketchLine_10 = Sketch_2.addLine(-64.56856594464165, -5.323114700643004, 61.72045613220821, -5.323114700643004)
SketchLine_11 = Sketch_2.addLine(61.72045613220821, -5.323114700643004, 61.72045613220821, 51.92790864086226)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_8.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_9.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_10.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_11.result())
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_8r-SketchLine_9f-SketchLine_10f-SketchLine_11f")])
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Revolution_1_1"), model.selection("FACE", "Face_1_1")])
model.do()

model.checkResult(Partition_1,model,1,[4],[1],[16],[82],[164])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

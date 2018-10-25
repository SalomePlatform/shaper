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

# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(-18.11540697674418, -21.53002906976744, 69.96519983648402)
SketchCircle_2 = Sketch_1.addCircle(-18.11540697674418, -21.53002906976744, 31.60961705559024)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchCircle_2.center())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_2_2r")], model.selection(), 100, 0)
Point_2 = model.addPoint(Part_1_doc, 100, 40, 80)
Point_3 = model.addPoint(Part_1_doc, 100, -40, 80)
Point_4 = model.addPoint(Part_1_doc, 20, 40, -80)
Plane_4 = model.addPlane(Part_1_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"), model.selection("VERTEX", "Point_3"))
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchLine_1 = Sketch_2.addLine(98.50650093667804, 100.9440062780326, 111.3990833103819, -44.73923695837706)
SketchLine_2 = Sketch_2.addLine(111.3990833103819, -44.73923695837706, -130.2032518739455, -87.67357443122384)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_2.addLine(-130.2032518739455, -87.67357443122384, -102.5665996005733, 115.6113578178721)
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_2.addLine(-102.5665996005733, 115.6113578178721, 98.50650093667804, 100.9440062780326)
SketchConstraintCoincidence_4 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
model.do()
Face_1_objects = [model.selection("EDGE", "Sketch_2/SketchLine_4"), model.selection("EDGE", "Sketch_2/SketchLine_1"), model.selection("EDGE", "Sketch_2/SketchLine_2"), model.selection("EDGE", "Sketch_2/SketchLine_3")]
Face_1 = model.addFace(Part_1_doc, Face_1_objects)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1"), model.selection("FACE", "Face_1_1")])
model.do()

model.checkResult(Partition_1,model,1,[3],[2],[10],[30],[60])
model.testHaveNamingSubshapes(Partition_1,model,Part_1_doc)

model.end()

assert(model.checkPythonDump())

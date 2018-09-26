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
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_1 = SketchProjection_2.createdFeature()
SketchArc_1 = Sketch_1.addArc(0, 0, 0, 39.7470266529203, 0, -39.7470266529203, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchArc_1.center())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.result(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.result())
SketchLine_2 = Sketch_1.addLine(0, -39.7470266529203, 0, 39.7470266529203)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.endPoint())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Revolution_1_1"), model.selection("FACE", "PartSet/XOY")])
model.do()
model.testHaveNamingSubshapes(Partition_1, model, Part_1_doc)
model.end()

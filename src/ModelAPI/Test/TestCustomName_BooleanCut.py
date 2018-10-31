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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(70, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 50)
SketchLine_3 = Sketch_1.addLine(0, 50, 70, 50)
SketchLine_4 = Sketch_1.addLine(70, 50, 70, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), 70)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), 50)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
Extrusion_1.result().setName("box")
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r"))
SketchLine_5 = Sketch_2.addLine(10, 55, 50, 55)
SketchLine_6 = Sketch_2.addLine(50, 55, 50, -5)
SketchLine_7 = Sketch_2.addLine(50, -5, 10, -5)
SketchLine_8 = Sketch_2.addLine(10, -5, 10, 55)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistanceVertical_1 = Sketch_2.setVerticalDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_7.endPoint(), 5)
SketchConstraintDistanceHorizontal_1 = Sketch_2.setHorizontalDistance(SketchLine_8.startPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), 10)
SketchConstraintLength_3 = Sketch_2.setLength(SketchLine_8.result(), 60)
SketchConstraintLength_4 = Sketch_2.setLength(SketchLine_7.result(), 40)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r")], model.selection(), 15, 5)
Extrusion_2.result().setName("tool")
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "box")], [model.selection("SOLID", "tool")])
model.do()

assert(Cut_1.result().name() == Extrusion_1.result().name()), "Boolean CUT name '{}' != '{}'".format(Cut_1.result().name(), Extrusion_1.result().name())

model.end()

assert(model.checkPythonDump())

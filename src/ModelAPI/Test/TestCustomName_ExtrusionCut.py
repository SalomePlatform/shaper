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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(50.00000000000001, 0, 0, 0)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchPoint_1.result())
SketchLine_2 = Sketch_1.addLine(0, 0, 0, 50)
SketchLine_3 = Sketch_1.addLine(0, 50, 50, 50)
SketchLine_4 = Sketch_1.addLine(50, 50, 50.00000000000001, 0)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 50)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 20, 0)
Extrusion_1.result().setName("plate")
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 0, 10, [model.selection("SOLID", "plate")])
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "plate/To_Face"))
SketchCircle_1 = Sketch_2.addCircle(30.00000000000001, 15, 10)
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[plate/Generated_Face&Sketch_1/SketchLine_4][plate/To_Face]"), False)
SketchLine_5 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_5.result(), 20, True)
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[plate/Generated_Face&Sketch_1/SketchLine_1][plate/To_Face]"), False)
SketchLine_6 = SketchProjection_3.createdFeature()
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchCircle_1.center(), SketchLine_6.result(), 15, True)
SketchConstraintRadius_1 = Sketch_2.setRadius(SketchCircle_1.results()[1], 10)
ExtrusionCut_1.setNestedSketch(Sketch_2)
model.do()

assert(ExtrusionCut_1.result().name() == Extrusion_1.result().name()), "ExtrusionCut name '{}' != '{}'".format(ExtrusionCut_1.result().name(), Extrusion_1.result().name())

model.end()

assert(model.checkPythonDump())

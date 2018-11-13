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
lenParam = model.addParameter(Part_1_doc, "len", "500")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-302.4220649391276, 58.25973730731688, -52.42206493912754, 58.25973730731688)
SketchLine_2 = Sketch_1.addLine(-52.42206493912754, 58.25973730731688, -52.42206493912754, -61.38418346467283)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-52.42206493912754, -61.38418346467283, 147.5779350608725, -61.38418346467283)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(147.5779350608725, -61.38418346467283, 147.5779350608725, 58.25973730731685)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(147.5779350608725, 58.25973730731685, 297.5779350608724, 58.25973730731685)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(297.5779350608724, 58.25973730731685, 297.5779350608724, -192.2459441196271)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(297.5779350608724, -192.2459441196271, -302.4220649391276, -192.2459441196271)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(-302.4220649391276, -192.2459441196271, -302.4220649391276, 58.25973730731688)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_8.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_7.result(), 600)
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_5.result(), SketchLine_1.result())
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_1.result(), 250)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_3.result(), 200)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_8r-SketchLine_7r-SketchLine_6r-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 600, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1"))
SketchLine_9 = Sketch_2.addLine(236.9930310780426, -200, -144.601680073095, -200)
SketchLine_10 = Sketch_2.addLine(-144.601680073095, -200, -144.601680073095, -500)
SketchLine_11 = Sketch_2.addLine(-144.601680073095, -500, 236.9930310780426, -500)
SketchLine_12 = Sketch_2.addLine(236.9930310780426, -500, 236.9930310780426, -200)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_5 = Sketch_2.setVertical(SketchLine_10.result())
SketchConstraintHorizontal_5 = Sketch_2.setHorizontal(SketchLine_11.result())
SketchConstraintVertical_6 = Sketch_2.setVertical(SketchLine_12.result())
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Sketch_1/SketchLine_1"), False)
SketchLine_13 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_13.result(), SketchLine_9.endPoint(), 200, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchLine_10.endPoint(), SketchLine_13.result(), "len", True)
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_9r-SketchLine_10f-SketchLine_11f-SketchLine_12f")], model.selection(), model.selection(), 0, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_7"), 0, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3&weak_name_1"))
SketchCircle_1 = Sketch_3.addCircle(27.57793506087247, -80, 42.03963933117138)
SketchProjection_2 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_1_1/From_Face][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3&weak_name_1]"), False)
SketchProjection_2.setName("SketchProjection_3")
SketchProjection_2.result().setName("SketchProjection_3")
SketchLine_14 = SketchProjection_2.createdFeature()
SketchLine_14.setName("SketchLine_15")
SketchLine_14.result().setName("SketchLine_15")
SketchConstraintDistance_4 = Sketch_3.setDistance(SketchLine_14.result(), SketchCircle_1.center(), 80, True)
SketchConstraintDistance_4.setName("SketchConstraintDistance_4")
SketchProjection_3 = Sketch_3.addProjection(model.selection("EDGE", "[(ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_1)(Extrusion_1_1/From_Face)(ExtrusionCut_1_1/Generated_Face&Sketch_2/SketchLine_9)][ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_3&weak_name_1]"), False)
SketchProjection_3.setName("SketchProjection_4")
SketchProjection_3.result().setName("SketchProjection_4")
SketchLine_15 = SketchProjection_3.createdFeature()
SketchLine_15.setName("SketchLine_16")
SketchLine_15.result().setName("SketchLine_16")
SketchConstraintDistance_3 = Sketch_3.setDistance(SketchLine_15.result(), SketchCircle_1.center(), 80, True)
SketchConstraintDistance_3.setName("SketchConstraintDistance_3")
model.do()
ExtrusionCut_2 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", "Sketch_3/Face-SketchCircle_1_2r_wire")], model.selection(), model.selection(), 0, model.selection("FACE", "ExtrusionCut_1_1/Modified_Face&Sketch_1/SketchLine_7"), 0, [model.selection("SOLID", "ExtrusionCut_1_1")])
model.do()
# update the sketch 2 to make split of a size of a box
lenParam.setValue(700)
model.end()

# check that sketch_3 and extrusion are updated well
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(ExtrusionCut_2.feature()))

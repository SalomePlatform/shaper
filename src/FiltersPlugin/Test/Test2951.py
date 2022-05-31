# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 0, 25)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
SketchLine_1 = Sketch_1.addLine(-35, 23, -35, -23)
SketchLine_2 = Sketch_1.addLine(-35, -23, -28, -23)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-28, -23, -23, -28)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-23, -28, -23, -35)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(-23, -35, 23, -35)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(23, -35, 23, -28)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchLine_7 = Sketch_1.addLine(23, -28, 28, -23)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(28, -23, 35, -23)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(35, -23, 35, 23)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchLine_10 = Sketch_1.addLine(35, 23, 28, 23)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchLine_11 = Sketch_1.addLine(28, 23, 23, 28)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_11.startPoint())
SketchLine_12 = Sketch_1.addLine(23, 28, 23, 35)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_1.addLine(23, 35, -23, 35)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_1.addLine(-23, 35, -23, 28)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchLine_15 = Sketch_1.addLine(-23, 28, -28, 23)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())
SketchLine_16 = Sketch_1.addLine(-28, 23, -35, 23)
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_17 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_16.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_9.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_13.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_2.result(), SketchLine_8.result())
SketchConstraintHorizontal_4 = Sketch_1.setHorizontal(SketchLine_16.result())
SketchConstraintCollinear_2 = Sketch_1.setCollinear(SketchLine_16.result(), SketchLine_10.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintCollinear_3 = Sketch_1.setCollinear(SketchLine_4.result(), SketchLine_14.result())
SketchConstraintVertical_4 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintCollinear_4 = Sketch_1.setCollinear(SketchLine_6.result(), SketchLine_12.result())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_4.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_16.result(), SketchLine_14.result())
SketchConstraintEqual_3 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_16.result())
SketchConstraintEqual_4 = Sketch_1.setEqual(SketchLine_14.result(), SketchLine_12.result())
SketchConstraintEqual_5 = Sketch_1.setEqual(SketchLine_12.result(), SketchLine_10.result())
SketchConstraintEqual_6 = Sketch_1.setEqual(SketchLine_10.result(), SketchLine_8.result())
SketchConstraintEqual_7 = Sketch_1.setEqual(SketchLine_6.result(), SketchLine_4.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_9.result(), 35, True)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 35, True)
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_5.result(), 35, True)
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_13.result(), 35, True)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 25)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 7)
SketchConstraintDistance_5 = Sketch_1.setDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchLine_4.result(), 23, True)
SketchConstraintDistance_6 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_2.result(), 23, True)
SketchConstraintDistance_7 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_12.result(), 23, True)
SketchConstraintDistance_8 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_16.result(), 23, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f-SketchLine_7f-SketchLine_8f-SketchLine_9f-SketchLine_10f-SketchLine_11f-SketchLine_12f-SketchLine_13f-SketchLine_14f-SketchLine_15f-SketchLine_16f-SketchCircle_1_2r")], model.selection(), 10, 0)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "OppositeToEdge", args = [model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_9][Extrusion_1_1/To_Face]")])])
model.end()

from GeomAPI import *

Reference = {}
ExtrusionResult = Extrusion_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ExtrusionResult.shape(), GeomAPI_Shape.EDGE)
# edges on face 1
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 2
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 3
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 4
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 5
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 6
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 7
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 8
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 9
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = True;  exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = True;  exp.next()
# edges on face 10
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 11
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 12
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 13
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 14
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 15
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 16
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 17
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 18
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = True;  exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
# edges on face 19
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = True;  exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
Reference[model.selection(ExtrusionResult, exp.current())] = False; exp.next()
assert(not exp.more())

model.checkFilter(Part_1_doc, model, Filters, Reference)

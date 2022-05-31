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

from salome.shaper import model
from GeomAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [], model.selection(), 100, 0)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(0, 0, 80)
SketchLine_1 = Sketch_1.addLine(-70.03988514133886, -38.65765757761403, 65.03214823048954, -46.59205615260649)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
SketchLine_2 = Sketch_1.addLine(6.89273184263731, -79.70251092497334, 48.24895637932232, 63.81252391424628)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_1.results()[1])
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 80)
Extrusion_1.setNestedSketch(Sketch_1)
Revolution_1 = model.addRevolution(Part_1_doc, [], model.selection("EDGE", "PartSet/OZ"), 270, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_3 = Sketch_2.addLine(80, -20, 20, -20)
SketchLine_4 = Sketch_2.addLine(20, -20, 20, -100)
SketchLine_5 = Sketch_2.addLine(20, -100, 80, -100)
SketchLine_6 = Sketch_2.addLine(80, -100, 80, -20)
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_6.result())
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_6.result(), 80, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_4.result(), 20, True)
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_3.result(), 20, True)
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_5.result(), 100, True)
Revolution_1.setNestedSketch(Sketch_2)
model.do()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", args = [model.selection("FACE", "Extrusion_1_1_3/To_Face")])])
model.end()

Reference = {}
# Extrusion
ResultExtrusion = Extrusion_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultExtrusion.shape(), GeomAPI_Shape.FACE)
# faces of solid 1
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# faces of solid 2
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# faces of solid 3
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# faces of solid 4
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# Revolution
ResultRevolution = Revolution_1.result().resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultRevolution.shape(), GeomAPI_Shape.FACE)
Reference[model.selection(ResultRevolution, exp.current())] = False; exp.next()
Reference[model.selection(ResultRevolution, exp.current())] = False; exp.next()
Reference[model.selection(ResultRevolution, exp.current())] = False; exp.next()
Reference[model.selection(ResultRevolution, exp.current())] = False; exp.next()
Reference[model.selection(ResultRevolution, exp.current())] = False; exp.next()
Reference[model.selection(ResultRevolution, exp.current())] = False; exp.next()

model.checkFilter(Part_1_doc, model, Filters, Reference)

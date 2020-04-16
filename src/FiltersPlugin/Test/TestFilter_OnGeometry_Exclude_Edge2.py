# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
model.do()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "OnGeometry", exclude = True, args = [model.selection("EDGE", "[Extrusion_1_1_3/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_3/To_Face]")])])
model.end()

Reference = {}
# Extrusion Solid 1
ResultExtrusion = Extrusion_1.result().subResult(0).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultExtrusion.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
# edges of face 3
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 4
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 6
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
assert(not exp.more())

# Extrusion Solid 2
ResultExtrusion = Extrusion_1.result().subResult(1).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultExtrusion.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 4
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
assert(not exp.more())

# Extrusion Solid 3
ResultExtrusion = Extrusion_1.result().subResult(2).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultExtrusion.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 4
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
assert(not exp.more())

# Extrusion Solid 4
ResultExtrusion = Extrusion_1.result().subResult(3).resultSubShapePair()[0]
exp = GeomAPI_ShapeExplorer(ResultExtrusion.shape(), GeomAPI_Shape.EDGE)
# edges of face 1
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
# edges of face 2
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 3
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 4
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
# edges of face 5
Reference[model.selection(ResultExtrusion, exp.current())] = False; exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
Reference[model.selection(ResultExtrusion, exp.current())] = True;  exp.next()
assert(not exp.more())

model.checkFilter(Part_1_doc, model, Filters, Reference)

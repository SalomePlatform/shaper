# Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_1.addCircle(0, -100, 20)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 20)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchAPI_Line(SketchLine_1).startPoint(), 100, True)
model.do()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), model.selection("EDGE", "PartSet/OX"), 30)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("FACE", "Plane_1"), 0, model.selection(), 0)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]"), True)
SketchCircle_2 = SketchProjection_2.createdFeature()
model.do()
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_3 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/From_Face]"), True)
SketchEllipse_1 = SketchProjection_3.createdFeature()
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "[Extrusion_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1/To_Face]"), True)
SketchEllipse_2 = SketchProjection_4.createdFeature()
model.do()
model.end()

from GeomAPI import *

circle = SketchCircle_2.results()[-1].resultSubShapePair()[0].shape()
assert(circle.isEdge() and circle.edge().isCircle())
ellipse1 = SketchEllipse_1.results()[-1].resultSubShapePair()[0].shape()
assert(ellipse1.isEdge() and ellipse1.edge().isEllipse())
ellipse2 = SketchEllipse_2.results()[-1].resultSubShapePair()[0].shape()
assert(ellipse2.isEdge() and ellipse2.edge().isEllipse())

assert(model.checkPythonDump())

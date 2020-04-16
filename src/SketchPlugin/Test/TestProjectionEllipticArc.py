# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchLine_2 = Sketch_1.addLine(-17.32050807568878, -110, 0, -100)
SketchLine_3 = Sketch_1.addLine(0, -100, 10, -117.3205080756888)
SketchArc_1 = Sketch_1.addArc(0, -100, 10, -117.3205080756888, -17.32050807568878, -110, False)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 20)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.center(), SketchAPI_Line(SketchLine_1).startPoint(), 100, True)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_2.endPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_2.result(), SketchLine_3.result())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_1.result(), 150)
model.do()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOZ"), model.selection("EDGE", "PartSet/OX"), 30)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), model.selection("FACE", "Plane_1"), 0, model.selection(), 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/To_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2]"), True)
SketchEllipticArc_1 = SketchProjection_2.createdFeature()
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "[Extrusion_1_1/From_Face][Extrusion_1_1/Generated_Face&Sketch_1/SketchArc_1_2]"), True)
SketchEllipticArc_2 = SketchProjection_3.createdFeature()
model.do()
model.end()

from GeomAPI import *

ellipse1 = SketchEllipticArc_1.results()[-1].resultSubShapePair()[0].shape()
assert(ellipse1.isEdge() and ellipse1.edge().isEllipse())
ellipse2 = SketchEllipticArc_2.results()[-1].resultSubShapePair()[0].shape()
assert(ellipse2.isEdge() and ellipse2.edge().isEllipse())

assert(model.checkPythonDump())

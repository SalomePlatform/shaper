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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchCircle_1 = Sketch_1.addCircle(-29.162567194092, -4.973946236013575, 24.14811574522286)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_2.addLine(-77.4631035991525, 93.73498685458384, -26.22597517262966, 46.04636096467877)
SketchLine_2 = Sketch_2.addLine(-26.22597517262966, 46.04636096467877, 72.15826607546271, 123.5085570887089)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_2.addLine(72.15826607546271, 123.5085570887089, -77.4631035991525, 93.73498685458384)
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_2.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f")], model.selection(), 50, 50)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_2"))
SketchProjection_1 = Sketch_3.addProjection(model.selection("EDGE", "Sketch_1/SketchCircle_1_2"), True)
SketchEllipse_1 = SketchProjection_1.createdFeature()
SketchLine_4 = Sketch_3.addLine(-38.4752072906812, 8.839586602068252, -81.23746009846235, -13.43622256955389)
SketchConstraintCoincidence_4 = Sketch_3.setCoincident(SketchLine_4.startPoint(), SketchEllipse_1.result())
SketchLine_5 = Sketch_3.addLine(-81.23746009846235, -13.43622256955389, -38.77350759536917, -18.22647561616226)
SketchConstraintCoincidence_5 = Sketch_3.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_3.setCoincident(SketchLine_5.endPoint(), SketchEllipse_1.result())
model.do()
model.end()

model.checkSketch(Sketch_3, 4)
model.testNbSubFeatures(Sketch_3, "SketchConstraintCoincidence", 3)
ellipse1 = SketchEllipse_1.results()[-1].resultSubShapePair()[0].shape()
assert(ellipse1.isEdge() and ellipse1.edge().isEllipse())

model.begin()
Sketch_3.setPlane(model.selection("FACE", "Extrusion_1_1/Generated_Face&Sketch_2/SketchLine_1"))
model.end()
model.checkSketch(Sketch_3, 4)
model.testNbSubFeatures(Sketch_3, "SketchConstraintCoincidence", 3)
ellipse1 = SketchEllipse_1.results()[-1].resultSubShapePair()[0].shape()
assert(ellipse1.isEdge() and ellipse1.edge().isEllipse())

assert(model.checkPythonDump())

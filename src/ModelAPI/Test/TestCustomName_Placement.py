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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(30, 76.16895059675974, 30, 16.16895059675975)
SketchLine_2 = Sketch_1.addLine(30, 16.16895059675975, 55.98076211353315, 31.16895059675975)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(55.98076211353315, 31.16895059675975, 30, 76.16895059675974)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_2.result(), SketchLine_3.result())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_1.result(), SketchLine_3.result(), 30.00000000000014)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 60)
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_4 = SketchProjection_1.createdFeature()
SketchConstraintMirror_1_objects = [SketchLine_2.result(), SketchLine_1.result(), SketchLine_3.result()]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_4.result(), SketchConstraintMirror_1_objects)
[SketchLine_5, SketchLine_6, SketchLine_7] = SketchConstraintMirror_1.mirrored()
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_2.startPoint(), SketchLine_4.result(), 30, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f")], model.selection(), 20, 0)
Extrusion_1.result().setName("prism")
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_7r-SketchLine_5r")], model.selection("EDGE", "Sketch_1/SketchLine_6"), 360, 0)
Revolution_1.result().setName("spinner")
Placement_1 = model.addPlacement(Part_1_doc, [model.selection("SOLID", "spinner")], model.selection("VERTEX", "[spinner/Generated_Face&Sketch_1/SketchLine_7][spinner/Generated_Face&Sketch_1/SketchLine_5]"), model.selection("FACE", "prism/Generated_Face&Sketch_1/SketchLine_2"), False, True)
model.do()

assert(Placement_1.result().name() == Revolution_1.result().name()), "Placement name '{}' != '{}'".format(Placement_1.result().name(), Revolution_1.result().name())

model.end()

assert(model.checkPythonDump())

# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(55.30809111108014, 52.50335568234566, -47.4076783683968, 52.50335568234566)
SketchLine_2 = Sketch_1.addLine(-47.4076783683968, 52.50335568234566, -77.7205049049542, 0)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-77.7205049049542, 0, -47.4076783683968, -52.50335568234565)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-47.4076783683968, -52.50335568234565, 55.30809111108014, -52.50335568234565)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(55.30809111108014, -52.50335568234565, 55.30809111108014, 52.50335568234566)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.result(), SketchLine_3.startPoint())
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_2.result(), SketchLine_3.result(), 120)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchLine_2.result(), SketchLine_3.result())
SketchConstraintEqual_2 = Sketch_1.setEqual(SketchLine_1.result(), SketchLine_4.result())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_3"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchLine_4"), model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_5")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects)
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection(), 10, 0)
model.do()

model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)

model.end()

assert(model.checkPythonDump())

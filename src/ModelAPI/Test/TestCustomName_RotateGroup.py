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
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Cylinder_1.result().setName("cylinder")
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchArc_1 = Sketch_1.addArc(13.72530642620941, 12.64599334927434, 17.72530642621026, 15.64599334928998, 17.72530642621026, 9.645993349258699, False)
SketchArc_2 = Sketch_1.addArc(21.72530642620941, 12.64599334927434, 17.72530642621026, 9.645993349258699, 17.72530642621026, 15.64599334928998, False)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchArc_2.endPoint())
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchLine_1 = Sketch_1.addLine(13.72530642620941, 12.64599334927434, 21.72530642620941, 12.64599334927434)
SketchLine_1.setAuxiliary(True)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.center(), SketchLine_1.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_2.center(), SketchLine_1.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchLine_1.startPoint())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 5)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_1.endPoint(), 8, True)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchArc_2_2f")], model.selection(), 10, 0)
Extrusion_1.result().setName("prism")
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "prism"), model.selection("SOLID", "cylinder")], model.selection("EDGE", "PartSet/OX"), 30)
model.do()

RotationResults = Rotation_1.results()
assert(RotationResults[0].name() == Extrusion_1.result().name()), "Rotation name [0] '{}' != '{}'".format(RotationResults[0].name(), Extrusion_1.result().name())
assert(RotationResults[1].name() == Cylinder_1.result().name()),  "Rotation name [1] '{}' != '{}'".format(RotationResults[1].name(), Cylinder_1.result().name())

model.end()

assert(model.checkPythonDump())

# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

# Checks that the Copy feature produces correct names, same as in the description #3109

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-11.9408866995074, 14.67733990147784, -29.35467980295567, 14.67733990147784)
SketchLine_2 = Sketch_1.addLine(-29.35467980295567, 14.67733990147784, -29.35467980295567, -7.960591133004924)
SketchLine_3 = Sketch_1.addLine(-29.35467980295567, -7.960591133004924, -11.9408866995074, -7.960591133004924)
SketchLine_4 = Sketch_1.addLine(-11.9408866995074, -7.960591133004924, -11.9408866995074, 14.67733990147784)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(14.92610837438425, 16.04556650246306, 6.602917012013241)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 10, 0)
Extrusion_1.result().setName("Box")
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f")], model.selection(), 10, 0)
Extrusion_2.result().setName("Cylinder")
Copy_1 = model.addCopy(Part_1_doc, [model.selection("SOLID", "Box"), model.selection("SOLID", "Cylinder")], 3)
model.end()

assert(Copy_1.feature().results().size() == 6)

assert(Copy_1.feature().results()[0].data().name() == "Box_1")
assert(Copy_1.feature().results()[1].data().name() == "Cylinder_1")
assert(Copy_1.feature().results()[2].data().name() == "Box_2")
assert(Copy_1.feature().results()[3].data().name() == "Cylinder_2")
assert(Copy_1.feature().results()[4].data().name() == "Box_3")
assert(Copy_1.feature().results()[5].data().name() == "Cylinder_3")

model.begin()
model.testHaveNamingSubshapes(Copy_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())

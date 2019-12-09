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

# Checks that the Copy feature produces correct results and names if sub-shapes of the same shape are selected

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-25.99630541871921, 19.52832512315271, -16.66748768472907, 29.72783251231528)
SketchLine_2 = Sketch_1.addLine(-16.66748768472907, 29.72783251231528, -12.4384236453202, 17.66256157635469)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-12.4384236453202, 17.66256157635469, -25.99630541871921, 19.52832512315271)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_3r-SketchLine_2r-SketchLine_1r")], model.selection(), 10, 0)
Extrusion_1.result().setName("Prism")
Copy_1_objects = [model.selection("FACE", "Prism/Generated_Face&Sketch_1/SketchLine_3"), model.selection("FACE", "Prism/Generated_Face&Sketch_1/SketchLine_2"), model.selection("EDGE", "[Prism/Generated_Face&Sketch_1/SketchLine_1][Prism/To_Face]")]
Copy_1 = model.addCopy(Part_1_doc, Copy_1_objects, 2)
Copy_1.result().setName("Prism_1")
model.end()

assert(Copy_1.feature().results().size() == 6)

for index in range(6):
  # name is just incremented
  assert(Copy_1.feature().results()[index].data().name() == "Prism_" + str(index + 1))
  # type of the shape corresponds to selection: 2 faces then edge
  if index%3 == 2:
    assert(Copy_1.feature().results()[index].shape().shapeTypeStr() == "EDGE")
  else:
    assert(Copy_1.feature().results()[index].shape().shapeTypeStr() == "FACE")

model.begin()
model.testHaveNamingSubshapes(Copy_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())

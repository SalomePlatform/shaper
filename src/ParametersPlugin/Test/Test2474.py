## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "l", "200")
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(138.3632494179343, 48.5641040948674, -61.63675058206567, 48.5641040948674)
SketchLine_2 = Sketch_1.addLine(-61.63675058206567, 48.5641040948674, -61.63675058206567, -46.51114810194107)
SketchLine_3 = Sketch_1.addLine(-61.63675058206567, -46.51114810194107, 138.3632494179343, -46.51114810194107)
SketchLine_4 = Sketch_1.addLine(138.3632494179343, -46.51114810194107, 138.3632494179343, 48.5641040948674)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), "l")
model.do()
model.addParameter(Part_1_doc, "l", "300")
model.end()

# Check that after substitution of module document "l" by part document "l" the value is updated
aValue = SketchConstraintLength_1.feature().real("ConstraintValue").value()
assert(aValue == 300)

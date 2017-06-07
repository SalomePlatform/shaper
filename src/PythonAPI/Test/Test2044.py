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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-184.375, -81.25, -71.87500000000006, 206.2500000000001)
SketchLine_2 = Sketch_1.addLine(-71.87500000000006, 206.2500000000001, 281.2500000000001, 178.125)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(281.2500000000001, 178.125, 184.3750000000001, -109.375)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(184.3750000000001, -109.375, -184.375, -81.25)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1r-SketchLine_2r-SketchLine_3r-SketchLine_4r")], model.selection("EDGE", "PartSet/OZ"), 50, 0)
model.do()
Extrusion_1.setDirection(model.selection("EDGE", "OX"))
model.do()
model.end()

assert (len(Extrusion_1.feature().error()) != 0)

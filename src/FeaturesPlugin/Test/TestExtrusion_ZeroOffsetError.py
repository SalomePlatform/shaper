## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
SketchCircle_1 = Sketch_1.addCircle(9.631110499908317, -35.9437937189679, 36.19447160731126)
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), 10, -10)
assert(Extrusion_1.feature().error() != "")

Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_2 = Sketch_2.addCircle(114.4867836253828, -21.50233830122853, 11.10986684682035)
SketchCircle_3 = Sketch_2.addCircle(113.4180342136906, 1.426992627069481, 10.29495051811322)
model.do()
Compound_1 = model.addCompound(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2r"), model.selection("FACE", "Sketch_2/Face-SketchCircle_3_2f")])

Plane_1 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/XOY"), 10, False)
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchCircle_4 = Sketch_3.addCircle(91.42492988686666, -20.30106607931821, 6.749165188215104)
SketchCircle_5 = Sketch_3.addCircle(88.32466240656791, 2.196618803039243, 8.273170230715275)
model.do()
Compound_2 = model.addCompound(Part_1_doc, [model.selection("FACE", "Sketch_3/Face-SketchCircle_4_2r"), model.selection("FACE", "Sketch_3/Face-SketchCircle_5_2f")])

Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), model.selection("COMPOUND", "Compound_1_1"), 0, model.selection("COMPOUND", "Compound_2_1"), 0)
assert(Extrusion_2.feature().error() != "")

model.end()

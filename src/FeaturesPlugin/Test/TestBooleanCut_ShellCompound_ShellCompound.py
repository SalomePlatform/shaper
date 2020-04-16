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
SketchCircle_1 = Sketch_1.addCircle(-174.3348611545506, 14.32227876658794, 70.49121821473575)
SketchLine_1 = Sketch_1.addLine(-170.8787982705978, 84.72872358712971, -177.6798961073988, -56.0895283864863)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
SketchCircle_2 = Sketch_1.addCircle(156.5938368884866, -3.269359348234124, 86.69415181338225)
SketchLine_2 = Sketch_1.addLine(159.264749945663, 83.38363934242156, 152.8285624017236, -89.88170642808817)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchCircle_2.results()[1])
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_2.results()[1])
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_3 = Sketch_2.addCircle(-63.56168377478743, 4.78844798363732, 75.55300890562218)
SketchCircle_4 = Sketch_2.addCircle(322.6564034385849, -22.92418256149703, 118.030609731015)
SketchLine_3 = Sketch_2.addLine(-138.9405630492767, 9.915017403336009, 11.9830331245263, 5.907779612586542)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchCircle_3.results()[1])
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchCircle_3.results()[1])
SketchLine_4 = Sketch_2.addLine(207.6267015097382, 3.521847530670962, 440.2647819790861, -32.89885701981125)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchCircle_4.results()[1])
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchCircle_4.results()[1])
model.do()
Shell_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r-SketchCircle_1_2r-SketchLine_1f"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchLine_2r"), model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_2_2f-SketchLine_2f")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Shell_2_objects = [model.selection("FACE", "Sketch_2/Face-SketchCircle_3_2r-SketchLine_3f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_3_2r-SketchCircle_3_2r-SketchLine_3r"), model.selection("FACE", "Sketch_2/Face-SketchCircle_4_2f-SketchCircle_4_2f-SketchLine_4f"), model.selection("FACE", "Sketch_2/Face-SketchCircle_4_2f-SketchLine_4r")]
Shell_2 = model.addShell(Part_1_doc, Shell_2_objects)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("SHELL", "Shell_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("SHELL", "Shell_2_1"), model.selection("SHELL", "Shell_2_2")])
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1")], [model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Cut_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [2])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [4])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [14])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [28])

assert(model.checkPythonDump())

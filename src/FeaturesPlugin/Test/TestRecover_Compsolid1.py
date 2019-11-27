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

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(31.9410018961801, -31.48668563219184, -26.28238891138439, -31.48668563219184)
SketchLine_2 = Sketch_1.addLine(-26.28238891138439, -31.48668563219184, -26.28238891138439, 32.16845339608692)
SketchLine_3 = Sketch_1.addLine(-26.28238891138439, 32.16845339608692, 31.9410018961801, 32.16845339608692)
SketchLine_4 = Sketch_1.addLine(31.9410018961801, 32.16845339608692, 31.9410018961801, -31.48668563219184)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchLine_5 = Sketch_1.addLine(-26.28238891138439, -12.06888707668976, 31.9410018961801, 21.15910771625288)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_2.result())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_4.result())
SketchLine_6 = Sketch_1.addLine(-0.02931756086478185, 2.913698204490415, 1.128717663020605, -31.48668563219184)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_5.result())
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_1.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 50, 0)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_4"))
SketchCircle_1 = Sketch_2.addCircle(-4.420087352215441, 24.04732854606771, 12.87676926472621)
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Face-SketchCircle_1_2r_wire")], model.selection(), 20, 80)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_2")], [model.selection("SOLID", "Extrusion_2_1")], keepSubResults = True)
Recover_1 = model.addRecover(Part_1_doc, Cut_1, [Extrusion_1.result()], True)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [6])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [41])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [182])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [364])
model.testResultsVolumes(Part_1, [354429.714290673])

assert(model.checkPythonDump())

# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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
SketchLine_1 = Sketch_1.addLine(-82.64093491686459, 27.4393729216152, -55.52437814726841, 47.45397434679335)
SketchLine_2 = Sketch_1.addLine(-55.52437814726841, 47.45397434679335, -29.69908598574823, 30.02190213776723)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-29.69908598574823, 30.02190213776723, -54.2331135391924, 7.424771496437049)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-54.2331135391924, 7.424771496437049, -82.64093491686459, 27.4393729216152)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-55.52437814726841, 47.45397434679335, -54.2331135391924, 7.424771496437049)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_6 = Sketch_2.addLine(-84.57783182897859, 11.29856532066509, -63.91759809976246, 28.73063752969122)
SketchLine_7 = Sketch_2.addLine(-63.91759809976246, 28.73063752969122, -60.68943657957245, -4.196609976247029)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_2.addLine(-60.68943657957245, -4.196609976247029, -84.57783182897859, -14.52672684085511)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_2.addLine(-84.57783182897859, -14.52672684085511, -84.57783182897859, 11.29856532066509)
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_6.startPoint(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_2.addLine(-84.57783182897859, 11.29856532066509, -60.68943657957245, -4.196609976247029)
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_6.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_10.endPoint())
model.do()
Shell_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchLine_4r-SketchLine_5r-SketchLine_1r"), model.selection("FACE", "Sketch_1/Face-SketchLine_5f-SketchLine_3r-SketchLine_2r"), model.selection("FACE", "Sketch_2/Face-SketchLine_10f-SketchLine_7r-SketchLine_6r"), model.selection("FACE", "Sketch_2/Face-SketchLine_10r-SketchLine_9r-SketchLine_8r")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("SHELL", "Shell_1_2")], True)
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [4])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [18])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [36])

assert(model.checkPythonDump())

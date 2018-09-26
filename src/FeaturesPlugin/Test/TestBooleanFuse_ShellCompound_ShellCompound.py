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
SketchLine_1 = Sketch_1.addLine(-83.2865672209026, 26.7937406175772, -53.58748123515441, 47.45397434679335)
SketchLine_2 = Sketch_1.addLine(-53.58748123515441, 47.45397434679335, -26.47092446555818, 25.50247600950118)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-26.47092446555818, 25.50247600950118, -53.58748123515441, 5.487874584323028)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-53.58748123515441, 5.487874584323028, -83.2865672209026, 26.7937406175772)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-53.58748123515441, 47.45397434679335, -53.58748123515441, 5.487874584323028)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(24.53402755344418, 35.83259287410926, 55.52437814726837, 51.32776817102136)
SketchLine_7 = Sketch_1.addLine(55.52437814726837, 51.32776817102136, 87.16036104513063, 32.60443135391922)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(87.16036104513063, 32.60443135391922, 53.58748123515437, 16.46362375296911)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(53.58748123515437, 16.46362375296911, 24.53402755344418, 35.83259287410926)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_1.addLine(55.52437814726837, 51.32776817102136, 53.58748123515437, 16.46362375296911)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_10.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_11 = Sketch_2.addLine(-85.86909643705464, 4.842242280285022, -62.62633349168645, 26.7937406175772)
SketchLine_12 = Sketch_2.addLine(-62.62633349168645, 26.7937406175772, -65.20886270783846, -2.259713064133024)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_2.addLine(-65.20886270783846, -2.259713064133024, -83.9321995249406, -14.52672684085511)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_2.addLine(-83.9321995249406, -14.52672684085511, -85.86909643705464, 4.842242280285022)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_14.endPoint())
SketchLine_15 = Sketch_2.addLine(-85.86909643705464, 4.842242280285022, -65.20886270783846, -2.259713064133024)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_15.endPoint())
SketchLine_16 = Sketch_2.addLine(31.63598289786226, 11.9441976247031, 44.54862897862235, 33.25006365795723)
SketchLine_17 = Sketch_2.addLine(44.54862897862235, 33.25006365795723, 52.29621662707837, 8.070403800475056)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_2.addLine(52.29621662707837, 8.070403800475056, 32.28161520190027, -12.58982992874109)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_2.addLine(32.28161520190027, -12.58982992874109, 31.63598289786226, 11.9441976247031)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchLine_19.endPoint())
SketchLine_20 = Sketch_2.addLine(31.63598289786226, 11.9441976247031, 52.29621662707837, 8.070403800475056)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_20.endPoint())
model.do()
Shell_1_objects = [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_4r-SketchLine_5r"), model.selection("FACE", "Sketch_1/Face-SketchLine_2r-SketchLine_3r-SketchLine_5f"), model.selection("FACE", "Sketch_1/Face-SketchLine_6r-SketchLine_9r-SketchLine_10r"), model.selection("FACE", "Sketch_1/Face-SketchLine_7r-SketchLine_8r-SketchLine_10f"), model.selection("FACE", "Sketch_2/Face-SketchLine_11r-SketchLine_12r-SketchLine_15f"), model.selection("FACE", "Sketch_2/Face-SketchLine_13r-SketchLine_14r-SketchLine_15r"), model.selection("FACE", "Sketch_2/Face-SketchLine_16r-SketchLine_17r-SketchLine_20f"), model.selection("FACE", "Sketch_2/Face-SketchLine_18r-SketchLine_19r-SketchLine_20r")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SHELL", "Shell_1_1"), model.selection("SHELL", "Shell_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("SHELL", "Shell_1_3"), model.selection("SHELL", "Shell_1_4")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [4])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [8])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [5])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [2])

assert(model.checkPythonDump())

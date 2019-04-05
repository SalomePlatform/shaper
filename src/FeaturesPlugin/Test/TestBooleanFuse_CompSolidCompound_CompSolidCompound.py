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
SketchLine_1 = Sketch_1.addLine(-81.42367066895368, 33.08919382504288, -49.89365351629502, 58.72898799313894)
SketchLine_2 = Sketch_1.addLine(-49.89365351629502, 58.72898799313894, -14.89879931389364, 32.04974271012008)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-14.89879931389364, 32.04974271012008, -45.04288164665522, 2.598627787307022)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-45.04288164665522, 2.598627787307022, -81.42367066895368, 33.08919382504288)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-49.89365351629502, 58.72898799313894, -45.04288164665522, 2.598627787307022)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(40.88507718696397, 37.93996569468268, 70.33619210977699, 65.31217838765009)
SketchLine_7 = Sketch_1.addLine(70.33619210977699, 65.31217838765009, 91.81818181818181, 33.43567753001717)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(91.81818181818181, 33.43567753001717, 59.24871355060034, 6.756432246998279)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(59.24871355060034, 6.756432246998279, 40.88507718696397, 37.93996569468268)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_1.addLine(70.33619210977699, 65.31217838765009, 59.24871355060034, 6.756432246998279)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_10.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_11 = Sketch_2.addLine(-24.25385934819896, -5.370497427101204, -36.03430531732418, 28.93138936535163)
SketchLine_12 = Sketch_2.addLine(-36.03430531732418, 28.93138936535163, -6.23670668953688, 16.80445969125213)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_2.addLine(-6.23670668953688, 16.80445969125213, -4.850771869639785, -14.37907375643225)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_2.addLine(-4.850771869639785, -14.37907375643225, -24.25385934819896, -5.370497427101204)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_14.endPoint())
SketchLine_15 = Sketch_2.addLine(-24.25385934819896, -5.370497427101204, -6.23670668953688, 16.80445969125213)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_15.endPoint())
SketchLine_16 = Sketch_2.addLine(32.22298456260721, 12.30017152658662, 55.78387650085761, 33.78216123499142)
SketchLine_17 = Sketch_2.addLine(55.78387650085761, 33.78216123499142, 51.27958833619211, 2.252144082332767)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_2.addLine(51.27958833619211, 2.252144082332767, 27.71869639794167, -15.41852487135506)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_2.addLine(27.71869639794167, -15.41852487135506, 32.22298456260721, 12.30017152658662)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchLine_19.endPoint())
SketchLine_20 = Sketch_2.addLine(32.22298456260721, 12.30017152658662, 51.27958833619211, 2.252144082332767)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_20.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 100, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 50, -20)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_2"), model.selection("COMPSOLID", "Extrusion_1_1")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_2_2"), model.selection("COMPSOLID", "Extrusion_2_1")])
Fuse_1 = model.addFuse(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")], True)
model.testHaveNamingSubshapes(Fuse_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Fuse_1, 1)
model.testNbSubResults(Fuse_1, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.FACE, [24])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.EDGE, [108])
model.testNbSubShapes(Fuse_1, GeomAPI_Shape.VERTEX, [216])

assert(model.checkPythonDump())

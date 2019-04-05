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
SketchLine_1 = Sketch_1.addLine(-20.89528643446688, 23.53376279136156, -12.17630887873155, 30.26420160631515)
SketchLine_2 = Sketch_1.addLine(-12.17630887873155, 30.26420160631515, -3.763260360039565, 22.15708212466651)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(-3.763260360039565, 22.15708212466651, -11.56445080464485, 13.8969981244962)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(-11.56445080464485, 13.8969981244962, -20.89528643446688, 23.53376279136156)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_4.endPoint())
SketchLine_5 = Sketch_1.addLine(-20.89528643446688, 23.53376279136156, -3.763260360039565, 22.15708212466651)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_5.endPoint())
SketchLine_6 = Sketch_1.addLine(7.862043047607545, 23.07486923579655, 15.35730445516949, 31.02902419892351)
SketchLine_7 = Sketch_1.addLine(15.35730445516949, 31.02902419892351, 24.53517556646984, 20.93336597649314)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(24.53517556646984, 20.93336597649314, 17.03991415890789, 12.67328197632283)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchLine_9 = Sketch_1.addLine(17.03991415890789, 12.67328197632283, 7.862043047607545, 23.07486923579655)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_9.endPoint())
SketchLine_10 = Sketch_1.addLine(7.862043047607545, 23.07486923579655, 24.53517556646984, 20.93336597649314)
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_10.endPoint())
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_11 = Sketch_2.addLine(-6.516621693429666, 11.60253034667112, -9.88184110090646, 19.09779175423307)
SketchLine_12 = Sketch_2.addLine(-9.88184110090646, 19.09779175423307, -1.162863545171131, 18.94482723571139)
SketchConstraintCoincidence_13 = Sketch_2.setCoincident(SketchLine_11.endPoint(), SketchLine_12.startPoint())
SketchLine_13 = Sketch_2.addLine(-1.162863545171131, 18.94482723571139, 0.8256751956106094, 13.43810456893119)
SketchConstraintCoincidence_14 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchLine_14 = Sketch_2.addLine(0.8256751956106094, 13.43810456893119, -6.516621693429666, 11.60253034667112)
SketchConstraintCoincidence_15 = Sketch_2.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_16 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_14.endPoint())
SketchLine_15 = Sketch_2.addLine(-1.162863545171131, 18.94482723571139, -6.516621693429666, 11.60253034667112)
SketchConstraintCoincidence_17 = Sketch_2.setCoincident(SketchLine_12.endPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_18 = Sketch_2.setCoincident(SketchLine_11.startPoint(), SketchLine_15.endPoint())
SketchLine_16 = Sketch_2.addLine(6.332397862390824, 16.34443042084296, 15.35730445516949, 18.79186271718972)
SketchLine_17 = Sketch_2.addLine(15.35730445516949, 18.79186271718972, 12.75690764030107, 10.22584967997607)
SketchConstraintCoincidence_19 = Sketch_2.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
SketchLine_18 = Sketch_2.addLine(12.75690764030107, 10.22584967997607, 6.179433343869151, 7.778417383629304)
SketchConstraintCoincidence_20 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchLine_19 = Sketch_2.addLine(6.179433343869151, 7.778417383629304, 6.332397862390824, 16.34443042084296)
SketchConstraintCoincidence_21 = Sketch_2.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_22 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchLine_19.endPoint())
SketchLine_20 = Sketch_2.addLine(6.332397862390824, 16.34443042084296, 12.75690764030107, 10.22584967997607)
SketchConstraintCoincidence_23 = Sketch_2.setCoincident(SketchLine_16.startPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_24 = Sketch_2.setCoincident(SketchLine_17.endPoint(), SketchLine_20.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 10, 10)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 5, 5)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1"), model.selection("COMPSOLID", "Extrusion_1_2")])
Compound_2 = model.addCompound(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_2_1"), model.selection("COMPSOLID", "Extrusion_2_2")])
Common_1 = model.addCommon(Part_1_doc, [model.selection("COMPOUND", "Compound_1_1"), model.selection("COMPOUND", "Compound_2_1")])
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.do()
model.end()

from GeomAPI import  GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [10])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [36])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [72])

assert(model.checkPythonDump())

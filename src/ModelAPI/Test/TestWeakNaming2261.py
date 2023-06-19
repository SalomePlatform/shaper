# Copyright (C) 2014-2023  CEA, EDF
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
SketchLine_1 = Sketch_1.addLine(500.0000311770457, 317.8294573643411, -308.1395348837211, 317.8294573643411)
SketchLine_2 = Sketch_1.addLine(-308.1395348837211, 317.8294573643411, -308.1395348837211, -263.9724263224183)
SketchLine_3 = Sketch_1.addLine(-308.1395348837211, -263.9724263224183, 500.0000311770457, -263.9724263224183)
SketchLine_4 = Sketch_1.addLine(500.0000311770457, -263.9724263224183, 500.0000311770457, 317.8294573643411)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
SketchCircle_1 = Sketch_1.addCircle(-118.2170542635659, 222.8682170542636, 141.685090902591)
SketchCircle_2 = Sketch_1.addCircle(294.5736434108529, -40.69767441860468, 113.2187726277406)
SketchArc_1 = Sketch_1.addArc(500.0000311770867, -335287.7996679137, 500.0000311770457, -263.9724263224183, -639.534183538502, -265.9104108427558, True)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_1.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_3.result(), SketchArc_1.results()[1])
SketchArc_2 = Sketch_1.addArc(-638.1195022375863, -681.8252609710054, -639.534183538502, -265.9104108427558, -316.014851248167, -418.6983758160058, True)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchArc_2.results()[1])
SketchArc_3 = Sketch_1.addArc(-167.5780303038277, -297.4405230733651, -316.014851248167, -418.6983758160058, -17.4411870157839, -178.2940378070349, False)
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchArc_3.startPoint())
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchArc_3.results()[1])
SketchArc_4 = Sketch_1.addArc(-301.8048538198685, -403.9610406236078, -17.4411870157839, -178.2940378070349, -637.5573748460384, -265.908384788777, False)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchArc_4.startPoint())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchArc_4.results()[1])
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchArc_2.results()[1], SketchArc_4.endPoint())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 100, 0)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPSOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OX"), 400)
model.do()
model.testHaveNamingSubshapes(Translation_1, model, Part_1_doc)
model.end()

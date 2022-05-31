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

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "a", "10")
model.addParameter(Part_1_doc, "b", "8")
model.addParameter(Part_1_doc, "c", "2*a")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-48.37423594569039, -43.07420401522882, -48.37423594569039, -8.074204015228814)
SketchLine_2 = Sketch_1.addLine(-40.38893648559045, -0.07421752185518632, -2.793802518897851e-22, 0)
SketchLine_3 = Sketch_1.addLine(-2.793802518897851e-22, 0, 37.58770483143633, -13.68080573302675)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_2")
SketchLine_4 = Sketch_1.addLine(37.58770483143633, -13.68080573302675, 19.91003530177265, -31.35847526269043)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_3")
SketchLine_5 = Sketch_1.addLine(-8.374235945690391, -43.07420401522882, -48.37423594569039, -43.07420401522882)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
SketchConstraintCoincidence_3.setName("SketchConstraintCoincidence_5")
SketchArc_1 = Sketch_1.addArc(-40.37423594569039, -8.074204015228817, -40.38893648559045, -0.07421752185518632, -48.37423594569039, -8.074204015228814, False)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4.setName("SketchConstraintCoincidence_6")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.endPoint())
SketchConstraintCoincidence_5.setName("SketchConstraintCoincidence_7")
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_1.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_2.result())
SketchArc_2 = Sketch_1.addArc(-8.374235945690213, -3.074204015227884, -8.374235945690391, -43.07420401522882, 19.91003530177265, -31.35847526269043, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6.setName("SketchConstraintCoincidence_8")
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_4.endPoint())
SketchConstraintCoincidence_7.setName("SketchConstraintCoincidence_9")
SketchConstraintTangent_3 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_4.result())
SketchConstraintTangent_4 = Sketch_1.setTangent(SketchArc_2.results()[1], SketchLine_5.result())
SketchLine_6 = Sketch_1.addLine(-2.793802518897851e-22, 0, 0, 35)
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_8.setName("SketchConstraintCoincidence_10")
SketchLine_7 = Sketch_1.addLine(30.00000000001173, 65.00000000001097, 42.00000000001173, 65.00000000001097)
SketchLine_8 = Sketch_1.addLine(65.00000000000007, 42, 65.00000000000007, 30)
SketchLine_9 = Sketch_1.addLine(35.00000000000009, 0, -2.793802518897851e-22, 0)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_9.endPoint())
SketchConstraintCoincidence_9.setName("SketchConstraintCoincidence_14")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_7.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_9.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_8.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_6.result())
SketchArc_3 = Sketch_1.addArc(30.00000000000873, 35.00000000000306, 30.00000000001173, 65.00000000001097, 0, 35, False)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchArc_3.startPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_10.setName("SketchConstraintCoincidence_15")
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchArc_3.endPoint(), SketchLine_6.endPoint())
SketchConstraintCoincidence_11.setName("SketchConstraintCoincidence_16")
SketchConstraintTangent_5 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_6.result())
SketchConstraintTangent_6 = Sketch_1.setTangent(SketchArc_3.results()[1], SketchLine_7.result())
SketchArc_4 = Sketch_1.addArc(42.00000000000008, 41.99999999999999, 65.00000000000007, 42, 42.00000000001173, 65.00000000001097, False)
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchArc_4.startPoint(), SketchLine_8.startPoint())
SketchConstraintCoincidence_12.setName("SketchConstraintCoincidence_17")
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchArc_4.endPoint(), SketchLine_7.endPoint())
SketchConstraintCoincidence_13.setName("SketchConstraintCoincidence_18")
SketchConstraintTangent_7 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_7.result())
SketchConstraintTangent_8 = Sketch_1.setTangent(SketchArc_4.results()[1], SketchLine_8.result())
SketchArc_5 = Sketch_1.addArc(35.00000000000006, 30.00000000000001, 35.00000000000009, 0, 65.00000000000007, 30, False)
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchArc_5.startPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_14.setName("SketchConstraintCoincidence_19")
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchArc_5.endPoint(), SketchLine_8.endPoint())
SketchConstraintCoincidence_15.setName("SketchConstraintCoincidence_20")
SketchConstraintTangent_9 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_8.result())
SketchConstraintTangent_10 = Sketch_1.setTangent(SketchArc_5.results()[1], SketchLine_9.result())
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_16 = Sketch_1.setCoincident(SketchLine_6.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintCoincidence_16.setName("SketchConstraintCoincidence_21")
SketchConstraintHorizontal_3 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_1.setVertical(SketchLine_1.result())
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_4.result(), SketchLine_3.result(), 65.00000000000011)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), "2*c")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), 25)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_5.result(), "4*a")
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_1.result(), 35)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 8)
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_9.result(), 20.00000000000001)
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 40)
SketchConstraintLength_5 = Sketch_1.setLength(SketchLine_6.result(), 35)
SketchConstraintLength_6 = Sketch_1.setLength(SketchLine_7.result(), 12)
SketchConstraintLength_7 = Sketch_1.setLength(SketchLine_8.result(), 12)
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchArc_3.results()[1], 30)
SketchConstraintRadius_4 = Sketch_1.setRadius(SketchArc_5.results()[1], 30)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), 10, 0)
model.do()
model.testHaveNamingSubshapes(Extrusion_1, model, Part_1_doc)
model.end()

assert(model.checkPythonDump())

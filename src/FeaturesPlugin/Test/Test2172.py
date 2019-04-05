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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(-85.62958302127123, 51.64856737222455, 159.4430553521271, 401.6485673722245)
SketchLine_2 = Sketch_1.addLine(159.4430553521271, 401.6485673722245, 909.4430553521272, 401.6485673722245)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchLine_3 = Sketch_1.addLine(909.4430553521272, 401.6485673722245, 909.4430553521272, -98.35143262777548)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchLine_4 = Sketch_1.addLine(909.4430553521272, -98.35143262777548, 551.916977573864, -398.3514326277755)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchLine_5 = Sketch_1.addLine(551.916977573864, -398.3514326277755, -18.08302242613596, -398.3514326277755)
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchLine_6 = Sketch_1.addLine(-18.08302242613596, -398.3514326277755, -18.08302242613596, -98.35143262777552)
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchArc_1 = Sketch_1.addArc(0, -7.626897158721915e-23, -85.62958302127123, 51.64856737222455, -18.08302242613596, -98.35143262777552, False)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchArc_1.endPoint())
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 100)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 750)
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_3.result(), 500)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_5.result(), 570)
SketchConstraintAngle_1 = Sketch_1.setAngleBackward(SketchLine_5.result(), SketchLine_4.result(), 139.9999999999999)
SketchConstraintAngle_2 = Sketch_1.setAngle(SketchLine_1.result(), SketchLine_2.result(), 124.9999999999987)
SketchConstraintLength_4 = Sketch_1.setLength(SketchLine_6.result(), 300)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchArc_1.startPoint(), SketchLine_2.result(), 350, False)
SketchConstraintDistance_2 = Sketch_1.setDistance(SketchLine_4.startPoint(), SketchLine_5.result(), 300, False)
SketchPoint_1 = Sketch_1.addPoint(model.selection("VERTEX", "PartSet/Origin"))
SketchConstraintCoincidence_8 = Sketch_1.setCoincident(SketchArc_1.center(), SketchPoint_1.coordinates())
SketchCircle_1 = Sketch_1.addCircle(623.3624504738834, 247.4315530304268, 75)
SketchCircle_2 = Sketch_1.addCircle(267.9975824521076, 55.86558171402226, 100)
SketchLine_7 = Sketch_1.addLine(-18.08302242613596, -98.35143262777552, 356.0223839531057, 103.3169707422677)
SketchConstraintCoincidence_9 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchLine_8 = Sketch_1.addLine(557.3438493481349, 211.8430112592427, 909.4430553521272, 401.6485673722245)
SketchConstraintCoincidence_10 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_8.endPoint())
SketchConstraintCoincidence_11 = Sketch_1.setCoincident(SketchCircle_2.center(), SketchLine_7.result())
SketchConstraintCoincidence_12 = Sketch_1.setCoincident(SketchCircle_1.center(), SketchLine_7.result())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchCircle_2.results()[1], 100)
SketchConstraintRadius_3 = Sketch_1.setRadius(SketchCircle_1.results()[1], 75)
SketchPoint_2 = Sketch_1.addPoint(557.3438493481349, 211.8430112592427)
SketchConstraintCoincidence_13 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchLine_8.startPoint())
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_7.result(), SketchLine_8.result())
SketchConstraintCoincidence_14 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchCircle_2.results()[1])
SketchConstraintDistance_3 = Sketch_1.setDistance(SketchCircle_2.center(), SketchLine_6.endPoint(), 325, False)
SketchConstraintCoincidence_15 = Sketch_1.setCoincident(SketchPoint_2.coordinates(), SketchCircle_1.results()[1])
SketchConstraintDistance_4 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_2.endPoint(), 325, False)
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchLine_8f")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_2_2f-SketchCircle_2_2f-SketchLine_7r")], model.selection("EDGE", "PartSet/OZ"), 360, 0)
Group_1_objects = [model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][weak_name_2]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][weak_name_1]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchCircle_1_2][weak_name_3]"), model.selection("EDGE", "[Revolution_1_1/Generated_Face&Sketch_1/SketchLine_8][weak_name_3]"), model.selection("EDGE", "[Revolution_2_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][weak_name_3]"), model.selection("EDGE", "[Revolution_2_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Revolution_2_1/Generated_Face&Sketch_1/SketchLine_7]"), model.selection("EDGE", "[Revolution_2_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_1][Revolution_2_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2]"), model.selection("EDGE", "[Revolution_2_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][weak_name_3]"), model.selection("EDGE", "[Revolution_2_1/Generated_Face&Sketch_1/SketchLine_7][weak_name_3]"), model.selection("EDGE", "[Revolution_2_1/Generated_Face&Sketch_1/SketchCircle_2_2&weak_name_2][Revolution_2_1/Generated_Face&Sketch_1/SketchLine_7]")]
Group_1 = model.addGroup(Part_1_doc, Group_1_objects)
model.do()
model.end()

# check that resulting group selection is valid
from ModelAPI import *
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(Group_1.feature()))
assert(Group_1.groupList().size() == 10)
for a in range(10):
  assert(Group_1.groupList().value(a).value().shapeTypeStr() == "EDGE")
  assert(len(Group_1.groupList().value(a).namingName()) > 0)

assert(model.checkPythonDump())

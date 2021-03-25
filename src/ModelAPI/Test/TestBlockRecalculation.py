# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
from timeit import default_timer as timer

# initial time value
startTime = timer()

model.begin()
partSet = model.moduleDocument()
param_l1 = model.addParameter(partSet, "l1", "20")
param_l2 = model.addParameter(partSet, "l2", "5")
param_a1 = model.addParameter(partSet, "a1", "35")
param_a2 = model.addParameter(partSet, "a2", "150")
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(0, 35.14523516719186, 10, 35.14523516719186)
SketchLine_2 = Sketch_1.addLine(10.57357643635423, 33.32608312288851, 6.47781621490927, 30.45820094113327)
SketchLine_3 = Sketch_1.addLine(6.47781621490927, 30.45820094113327, 3, 23)
SketchLine_4 = Sketch_1.addLine(3, 23, 3, 3)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_1.setName("SketchConstraintCoincidence_3")
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "OX"), False)
SketchLine_5 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_2.endPoint())
SketchConstraintCoincidence_2.setName("SketchConstraintCoincidence_4")
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "OY"), False)
SketchLine_6 = SketchProjection_2.createdFeature()
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_4.result(), "l1")
SketchArc_1 = Sketch_1.addArc(10.00000000000669, 34.14523516718319, 10.57357643635423, 33.32608312288851, 10, 35.14523516719186, False)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.endPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_1.result())
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_2.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 1)
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_3.result(), SketchLine_2.result(), "a2")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_2.result(), "l2")
SketchConstraintAngle_2 = Sketch_1.setAngleBackward(SketchLine_2.result(), SketchLine_1.result(), "a1")
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_4.endPoint(), SketchAPI_Line(SketchLine_6).startPoint(), 3)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_4.endPoint(), SketchAPI_Line(SketchLine_6).startPoint(), 3)
SketchConstraintLength_3 = Sketch_1.setLength(SketchLine_1.result(), "l1/2")
SketchConstraintMirror_1_objects = [SketchLine_1.result(), SketchArc_1.results()[1], SketchLine_2.result(), SketchLine_3.result(), SketchLine_4.result()]
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_6.result(), SketchConstraintMirror_1_objects)
[SketchLine_7, SketchArc_2, SketchLine_8, SketchLine_9, SketchLine_10] = SketchConstraintMirror_1.mirrored()
SketchMultiRotation_1_objects = [SketchLine_10.result(), SketchLine_9.result(), SketchLine_8.result(), SketchArc_2.results()[1], SketchLine_7.result(), SketchLine_1.result(), SketchArc_1.results()[1], SketchLine_2.result(), SketchLine_3.result(), SketchLine_4.result()]
SketchMultiRotation_1 = Sketch_1.addRotation(SketchMultiRotation_1_objects, SketchAPI_Line(SketchLine_6).startPoint(), 360, 4, True)
[SketchLine_10, SketchLine_11, SketchLine_12, SketchLine_13, SketchLine_9, SketchLine_14, SketchLine_15, SketchLine_16, SketchLine_8, SketchLine_17, SketchLine_18, SketchLine_19, SketchArc_2, SketchArc_3, SketchArc_4, SketchArc_5, SketchLine_7, SketchLine_20, SketchLine_21, SketchLine_22, SketchLine_23, SketchLine_24, SketchLine_25, SketchArc_6, SketchArc_7, SketchArc_8, SketchLine_26, SketchLine_27, SketchLine_28, SketchLine_29, SketchLine_30, SketchLine_31, SketchLine_32, SketchLine_33, SketchLine_34] = SketchMultiRotation_1.rotated()
model.do()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "PartSet/Sketch_1")], model.selection(), 5, 0)
Fillet_1 = model.addFillet(Part_1_doc, [model.selection("FACE", "Extrusion_1_1/To_Face_1"), model.selection("FACE", "Extrusion_1_1/From_Face_1")], 1)
model.end()

# creation of the model time
creationTime = timer() - startTime
print("Creation time: {0}".format(creationTime))

# disable automatic recalculation, so, modification of parameters cause norecomputation immediately
from ModelAPI import *
aSession = ModelAPI_Session.get()
aSession.blockAutoUpdate(True)

#update parameters one by one
model.begin()
param_l1.setValue(15)
model.do()
param_l2.setValue(3)
model.do()
param_a1.setValue(20)
model.do()
param_a2.setValue(145)
model.end()

aSession.blockAutoUpdate(False)

# compute update parameters and recomputation of the model time
modificationTime = timer() - startTime - creationTime
print("Modification time: {0}".format(modificationTime))

# check that volume was recomputed in accordance to parameters
model.testResultsVolumes(Fillet_1, [3242.917586105217651493148878217])

# check that 4 modification of parameters was not slower than 2 update of model (without block it is minimum 3 times of creation time)
assert(modificationTime < 2. * creationTime)

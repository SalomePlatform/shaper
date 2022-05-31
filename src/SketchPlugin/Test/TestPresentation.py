# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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

"""
    TestPresentation.py
    Test AIS presentations for constraints and macro-features
"""

from salome.shaper import model
from GeomAPI import *
from ModelAPI import *
from GeomDataAPI import *

from ConfigAPI import *
Config_PropManager().registerProp("Visualization", "dimension_value_size", "Dimension value size", Config_Prop.IntSpin, "16")
Config_PropManager().registerProp("Visualization", "sketch_dimension_color", "Dimension color", Config_Prop.Color, "64,128,255")

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-1.492165138365909, 9.860293583877349, 21.86930170264727, 26.80555863006795)
SketchLine_2 = Sketch_1.addLine(21.86930170264727, 26.80555863006795, 23.42558840321396, 11.88651135131589)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintAngle_1 = Sketch_1.setAngle(SketchLine_1.result(), SketchLine_2.result(), 60)
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchLine_1.startPoint(), SketchLine_2.endPoint(), 25, True)
SketchLine_3 = Sketch_1.addLine(23.50783486163409, 27.99407041552052, 34.45590553653152, 35.9352652297464)
SketchConstraintCollinear_1 = Sketch_1.setCollinear(SketchLine_1.result(), SketchLine_3.result())
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_1.startPoint(), SketchLine_3.startPoint(), 25)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_2.result(), 15)
SketchLine_4 = Sketch_1.addLine(53.74716193144771, -7.706985565972583, 35.95905823005196, -7.706985565972583)
SketchLine_5 = Sketch_1.addLine(35.95905823005196, -7.706985565972583, 35.95905823005196, 16.71916358123427)
SketchLine_6 = Sketch_1.addLine(35.95905823005196, 16.71916358123427, 53.74716193144771, 16.71916358123427)
SketchLine_7 = Sketch_1.addLine(53.74716193144771, 16.71916358123427, 53.74716193144771, -7.706985565972583)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_4.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_5.result())
SketchConstraintParallel_1 = Sketch_1.setParallel(SketchLine_5.result(), SketchLine_7.result())
SketchConstraintPerpendicular_1 = Sketch_1.setPerpendicular(SketchLine_6.result(), SketchLine_7.result())
SketchCircle_1 = Sketch_1.addCircle(20.43340729011699, -17.26699099051272, 10)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchConstraintRigid_1 = Sketch_1.setFixed(SketchCircle_1.center())
SketchArc_1 = Sketch_1.addArc(44.85311008074984, -7.706985565972585, 49.46781809225963, -16.57853954306742, 34.85430557643821, -7.861609252895618, False)
SketchConstraintEqual_1 = Sketch_1.setEqual(SketchCircle_1.results()[1], SketchArc_1.results()[1])
SketchConstraintMiddle_1 = Sketch_1.setMiddlePoint(SketchArc_1.center(), SketchLine_4.result())
SketchLine_8 = Sketch_1.addLine(49.46781809225963, -16.57853954306742, 32.21747944141746, -25.55163221418553)
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_8.startPoint())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchArc_1.results()[1], SketchLine_8.result())
SketchConstraintMirror_1 = Sketch_1.addMirror(SketchLine_1.result(), [SketchLine_2.result()])
SketchMultiTranslation_1 = Sketch_1.addTranslation([SketchLine_2.result()], SketchLine_3.startPoint(), SketchLine_3.endPoint(), 3, True)
SketchMultiRotation_1 = Sketch_1.addRotation([SketchLine_3.result()], SketchLine_3.endPoint(), 360, 4, True)
model.do()

assert(featureToPresentation(SketchConstraintAngle_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintCoincidence_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintCollinear_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintDistance_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintDistanceHorizontal_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintEqual_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintHorizontal_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintLength_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintMiddle_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintParallel_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintPerpendicular_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintRadius_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintRigid_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintTangent_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintVertical_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchConstraintMirror_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchMultiTranslation_1.feature()).getAISObject(None) is not None)
assert(featureToPresentation(SketchMultiRotation_1.feature()).getAISObject(None) is not None)

model.end()

# Test presentation for Fillet on low-level
aSession = ModelAPI_Session.get()
aSketchFeature = featureToCompositeFeature(Sketch_1.feature())
aSession.startOperation()
aFillet = aSketchFeature.addFeature("SketchFillet")
aFillet.refattr("fillet_point").setAttr(SketchLine_1.endPoint());
assert(featureToPresentation(aFillet).getAISObject(None) is not None)
aSession.finishOperation()

# Test presentation for MacroCircle on low-level
aSession.startOperation()
aCircle = aSketchFeature.addFeature("SketchMacroCircle")
aCirclePnt1 = geomDataAPI_Point2D(aCircle.attribute("first_point"))
aCirclePnt2 = geomDataAPI_Point2D(aCircle.attribute("second_point"))
aCirclePnt3 = geomDataAPI_Point2D(aCircle.attribute("third_point"))
aCircleType = aCircle.string("circle_type")
aCircleType.setValue("circle_type_by_three_points")
aCirclePnt1.setValue(10, 0)
aCirclePnt2.setValue(-10, 0)
aCirclePnt3.setValue(0, 10)
assert(featureToPresentation(aCircle).getAISObject(None) is not None)
aSession.finishOperation()

# Test presentation for MacroArc on low-level
aSession.startOperation()
anArc = aSketchFeature.addFeature("SketchMacroArc")
anArcPnt1 = geomDataAPI_Point2D(anArc.attribute("start_point_2"))
anArcPnt2 = geomDataAPI_Point2D(anArc.attribute("end_point_2"))
anArcPnt3 = geomDataAPI_Point2D(anArc.attribute("passed_point"))
anArcType = anArc.string("arc_type")
anArcType.setValue("by_three_points")
anArcPnt1.setValue(5, 0)
anArcPnt2.setValue(-5, 0)
anArcPnt3.setValue(0, 5)
assert(featureToPresentation(anArc).getAISObject(None) is not None)
aSession.finishOperation()

# Test presentation for MacroEllipse on low-level
aSession.startOperation()
anEllipse = aSketchFeature.addFeature("SketchMacroEllipse")
anEllipsePnt1 = geomDataAPI_Point2D(anEllipse.attribute("first_point"))
anEllipsePnt2 = geomDataAPI_Point2D(anEllipse.attribute("second_point"))
anEllipsePnt3 = geomDataAPI_Point2D(anEllipse.attribute("passed_point"))
anEllipseType = anEllipse.string("ellipse_type")
anEllipseType.setValue("by_center_axis_point")
anEllipsePnt1.setValue(10, 0)
anEllipsePnt2.setValue(-10, 0)
anEllipsePnt3.setValue(0, 5)
assert(featureToPresentation(anEllipse).getAISObject(None) is not None)
aSession.finishOperation()

# Test presentation for MacroEllipticArc on low-level
aSession.startOperation()
anEllipticArc = aSketchFeature.addFeature("SketchMacroEllipticArc")
anEllipticArcPnt1 = geomDataAPI_Point2D(anEllipticArc.attribute("center"))
anEllipticArcPnt2 = geomDataAPI_Point2D(anEllipticArc.attribute("major_axis_point"))
anEllipticArcPnt3 = geomDataAPI_Point2D(anEllipticArc.attribute("start_point"))
anEllipticArcPnt4 = geomDataAPI_Point2D(anEllipticArc.attribute("end_point"))
anEllipticArcPnt1.setValue(0, 0)
anEllipticArcPnt2.setValue(10, 0)
anEllipticArcPnt3.setValue(0, 5)
anEllipticArcPnt4.setValue(-10, 0)
assert(featureToPresentation(anEllipticArc).getAISObject(None) is not None)
aSession.finishOperation()

# Test presentation for MacroBSpline on low-level
aSession.startOperation()
aBSpline = aSketchFeature.addFeature("SketchMacroBSpline")
aPoles = geomDataAPI_Point2DArray(aBSpline.attribute("poles"))
aPoles.setSize(4)
aPoles.setPnt(0, 0, 0)
aPoles.setPnt(1, 10, 0)
aPoles.setPnt(2, 10, 10)
aPoles.setPnt(3, 0, 10)
aWeights = aBSpline.data().realArray("weights")
aWeights.setSize(4)
aWeights.setValue(0, 1)
aWeights.setValue(1, 2)
aWeights.setValue(2, 2)
aWeights.setValue(3, 1)
aBSpline.boolean("need_control_poly").setValue(True)
assert(featureToPresentation(aBSpline).getAISObject(None) is not None)
aSession.finishOperation()

# Test presentation for MacroBSplinePeriodic on low-level
aSession.startOperation()
aBSplineP = aSketchFeature.addFeature("SketchMacroBSplinePeriodic")
aPoles = geomDataAPI_Point2DArray(aBSplineP.attribute("poles"))
aPoles.setSize(4)
aPoles.setPnt(0, 0, 0)
aPoles.setPnt(1, 10, 0)
aPoles.setPnt(2, 10, 10)
aPoles.setPnt(3, 0, 10)
aWeights = aBSplineP.data().realArray("weights")
aWeights.setSize(4)
aWeights.setValue(0, 1)
aWeights.setValue(1, 2)
aWeights.setValue(2, 2)
aWeights.setValue(3, 1)
aBSplineP.boolean("need_control_poly").setValue(True)
assert(featureToPresentation(aBSplineP).getAISObject(None) is not None)
aSession.finishOperation()

## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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

from ConfigAPI import *
Config_PropManager().registerProp("Visualization", "dimension_value_size", "Dimension value size", Config_Prop.IntSpin, "16")
Config_PropManager().registerProp("Visualization", "dimension_arrow_size", "Dimension arrow size", Config_Prop.IntSpin, "16")
Config_PropManager().registerProp("Visualization", "dimension_font", "Dimension font", Config_Prop.String, "Times-bold")
Config_PropManager().registerProp("Visualization", "sketch_dimension_color", "Dimension color", Config_Prop.Color, "64,128,225")

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(-15.79157881408318, 17.46869043106789, -28.20135254460625, -14.21916935022164)
SketchLine_2 = Sketch_1.addLine(-28.20135254460625, -14.21916935022164, 5.332188506333114, -12.56026625936245)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchArc_1 = Sketch_1.addArc(-8.886980843888518, -0.1184930779185158, 5.332188506333114, -12.56026625936245, -15.79157881408318, 17.46869043106789, False)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchArc_1.endPoint())
model.do()
model.end()

from ModelAPI import *

aSession = ModelAPI_Session.get()
aSession.startOperation()
aMeasurement = Part_1_doc.addFeature("Measurement")
aMeasurement.string("MeasureKind").setValue("Length")
aMeasurement.selection("edge_for_length").selectSubShape("EDGE", "Sketch_1/SketchLine_1")
anAIS = featureToPresentation(aMeasurement).getAISObject(None)
assert(anAIS is not None)
anAIS = featureToPresentation(aMeasurement).getAISObject(anAIS)
assert(anAIS is not None)
aSession.finishOperation()

aSession.startOperation()
aMeasurement = Part_1_doc.addFeature("Measurement")
aMeasurement.string("MeasureKind").setValue("Distance")
aMeasurement.selection("distance_from").selectSubShape("VERTEX", "Sketch_1/SketchLine_1_StartVertex")
aMeasurement.selection("distance_to").selectSubShape("EDGE", "Sketch_1/SketchLine_2")
anAIS = featureToPresentation(aMeasurement).getAISObject(None)
assert(anAIS is not None)
anAIS = featureToPresentation(aMeasurement).getAISObject(anAIS)
assert(anAIS is not None)
aSession.finishOperation()

aSession.startOperation()
aMeasurement = Part_1_doc.addFeature("Measurement")
aMeasurement.string("MeasureKind").setValue("Radius")
aMeasurement.selection("circular").selectSubShape("EDGE", "Sketch_1/SketchArc_1_2")
anAIS = featureToPresentation(aMeasurement).getAISObject(None)
assert(anAIS is not None)
anAIS = featureToPresentation(aMeasurement).getAISObject(anAIS)
assert(anAIS is not None)
aSession.finishOperation()

aSession.startOperation()
aMeasurement = Part_1_doc.addFeature("Measurement")
aMeasurement.string("MeasureKind").setValue("Angle")
aMeasurement.selection("angle_from").selectSubShape("EDGE", "Sketch_1/SketchLine_1")
aMeasurement.selection("angle_to").selectSubShape("EDGE", "Sketch_1/SketchLine_2")
anAIS = featureToPresentation(aMeasurement).getAISObject(None)
assert(anAIS is not None)
anAIS = featureToPresentation(aMeasurement).getAISObject(anAIS)
assert(anAIS is not None)
aSession.finishOperation()

aSession.startOperation()
aMeasurement = Part_1_doc.addFeature("Measurement")
aMeasurement.string("MeasureKind").setValue("AngleBy3Points")
aMeasurement.selection("angle_point_1").selectSubShape("VERTEX", "Sketch_1/SketchLine_1_StartVertex")
aMeasurement.selection("angle_point_2").selectSubShape("VERTEX", "Sketch_1/SketchLine_1_EndVertex")
aMeasurement.selection("angle_point_3").selectSubShape("VERTEX", "Sketch_1/SketchLine_2_EndVertex")
anAIS = featureToPresentation(aMeasurement).getAISObject(None)
assert(anAIS is not None)
anAIS = featureToPresentation(aMeasurement).getAISObject(anAIS)
assert(anAIS is not None)
aSession.finishOperation()

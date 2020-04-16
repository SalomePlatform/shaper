# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
    TestProjection.py
    Unit test of SketchPlugin_Projection class

"""
from GeomDataAPI import *
from ModelAPI import *
import math
from salome.shaper import model

#=========================================================================
# Initialization of the test
#=========================================================================

__updated__ = "2016-05-16"


#=========================================================================
# Start of test
#=========================================================================
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
#=========================================================================
# Creation of a sketch
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Create a line, circle and arc
#=========================================================================
aSession.startOperation()
aLine = aSketchFeature.addFeature("SketchLine")
aLineStart = geomDataAPI_Point2D(aLine.attribute("StartPoint"))
aLineEnd = geomDataAPI_Point2D(aLine.attribute("EndPoint"))
aLineStart.setValue(10., 10.)
aLineEnd.setValue(40., 30.)

aCircle = aSketchFeature.addFeature("SketchCircle")
aCircleCenter = geomDataAPI_Point2D(aCircle.attribute("circle_center"))
aCircleRadius = aCircle.real("circle_radius")
aCircleCenter.setValue(-25., -25)
aCircleRadius.setValue(25.)

anArc = aSketchFeature.addFeature("SketchArc")
anArcCenter = geomDataAPI_Point2D(anArc.attribute("center_point"))
anArcStart = geomDataAPI_Point2D(anArc.attribute("start_point"))
anArcEnd = geomDataAPI_Point2D(anArc.attribute("end_point"))
anArcCenter.setValue(10., 10.)
anArcStart.setValue(50., 0.)
anArcEnd.setValue(0., 50.)
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 12)
#=========================================================================
# Create another sketch
#=========================================================================
aSession.startOperation()
aSketchFeature = featureToCompositeFeature(aDocument.addFeature("Sketch"))
origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 10)
dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSession.finishOperation()
#=========================================================================
# Project all features onto the new sketch
#=========================================================================
aSession.startOperation()
aLineProjector = aSketchFeature.addFeature("SketchProjection")
aLineProjector.selection("ExternalFeature").selectSubShape("EDGE", "Sketch_1/SketchLine_1")
aLineProjector.boolean("IncludeToResult").setValue(False)
aLineProjector.execute()

aCircleProjector = aSketchFeature.addFeature("SketchProjection")
aCircleProjector.selection("ExternalFeature").selectSubShape("EDGE", "Sketch_1/SketchCircle_1_2")
aCircleProjector.boolean("IncludeToResult").setValue(False)
aCircleProjector.execute()

anArcProjector = aSketchFeature.addFeature("SketchProjection")
anArcProjector.selection("ExternalFeature").selectSubShape("EDGE", "Sketch_1/SketchArc_1_2")
anArcProjector.boolean("IncludeToResult").setValue(False)
anArcProjector.execute()
aSession.finishOperation()
assert (model.dof(aSketchFeature) == 0)
#=========================================================================
# Check projection coordinates
#=========================================================================
aProjLine = ModelAPI_Feature.feature(aLineProjector.refattr("ProjectedFeature").object())
assert(aProjLine)
aProjLineStart = geomDataAPI_Point2D(aProjLine.attribute("StartPoint"))
aProjLineEnd = geomDataAPI_Point2D(aProjLine.attribute("EndPoint"))
assert(math.fabs(aProjLineStart.x() - aLineStart.x()) < 1.e-10)
assert(math.fabs(aProjLineStart.y() - aLineStart.y()) < 1.e-10)
assert(math.fabs(aProjLineEnd.x() - aLineEnd.x()) < 1.e-10)
assert(math.fabs(aProjLineEnd.y() - aLineEnd.y()) < 1.e-10)

aProjCircle = ModelAPI_Feature.feature(aCircleProjector.refattr("ProjectedFeature").object())
assert(aProjCircle)
aProjCircleCenter = geomDataAPI_Point2D(aProjCircle.attribute("circle_center"))
aProjCircleRadius = aProjCircle.real("circle_radius")
assert(math.fabs(aProjCircleCenter.x() - aCircleCenter.x()) < 1.e-10)
assert(math.fabs(aProjCircleCenter.y() - aCircleCenter.y()) < 1.e-10)
assert(math.fabs(aProjCircleRadius.value() - aCircleRadius.value()) < 1.e-10)

aProjArc = ModelAPI_Feature.feature(anArcProjector.refattr("ProjectedFeature").object())
aProjArcCenter = geomDataAPI_Point2D(aProjArc.attribute("center_point"))
aProjArcStart = geomDataAPI_Point2D(aProjArc.attribute("start_point"))
aProjArcEnd = geomDataAPI_Point2D(aProjArc.attribute("end_point"))
assert(math.fabs(aProjArcCenter.x() - anArcCenter.x()) < 1.e-10)
assert(math.fabs(aProjArcCenter.y() - anArcCenter.y()) < 1.e-10)
assert(math.fabs(aProjArcStart.x() - anArcStart.x()) < 1.e-10)
assert(math.fabs(aProjArcStart.y() - anArcStart.y()) < 1.e-10)
assert(math.fabs(aProjArcEnd.x() - anArcEnd.x()) < 1.e-10)
assert(math.fabs(aProjArcEnd.y() - anArcEnd.y()) < 1.e-10)
#=========================================================================
# Move original feature and check the projection is agreed
#=========================================================================
aSession.startOperation()
aLineStart.setValue(20., 0.)
aSession.finishOperation()
assert(math.fabs(aProjLineStart.x() - aLineStart.x()) < 1.e-10)
assert(math.fabs(aProjLineStart.y() - aLineStart.y()) < 1.e-10)
assert(math.fabs(aProjLineEnd.x() - aLineEnd.x()) < 1.e-10)
assert(math.fabs(aProjLineEnd.y() - aLineEnd.y()) < 1.e-10)
assert (model.dof(aSketchFeature) == 0)
#=========================================================================
# End of test
#=========================================================================

assert(model.checkPythonDump())

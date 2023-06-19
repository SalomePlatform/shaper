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

"""
      TestExtrusion.py
      Unit test of FeaturesPlugin_Boolean class: many Boolean operations performance

      class FeaturesPlugin_Extrusion : public ModelAPI_Feature
        static const std::string MY_EXTRUSION_ID("Extrusion");
        static const std::string MY_FACE_ID("base");
        static const std::string MY_SIZE_ID("size");
        static const std::string MY_REVERSE_ID("reverse");

        data()->addAttribute(FeaturesPlugin_Extrusion::FACE_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(FeaturesPlugin_Extrusion::SIZE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FeaturesPlugin_Extrusion::REVERSE_ID(), ModelAPI_AttributeBoolean::typeId());
"""

# Number rows and columns of cylinders that cuts the big box. Number of Boolena operations is N*N
N = 5

#=========================================================================
# Initialization of the test
#=========================================================================
from GeomAPI import *
from GeomAlgoAPI import *
from GeomDataAPI import *
from ModelAPI import *


__updated__ = "2015-03-26"

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for extrusion
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
# Another way is:
# aPart = aSession.activeDocument()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a list of sketches with one circle inside of each to extrude
#=========================================================================
step = 99. / N
radius = 95. / N / 2.

aSession.startOperation()
aSketchFeatures = []
for i in range(0, N):
    for j in range(0, N):
        # Create circle
        aSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
        origin = geomDataAPI_Point(aSketchFeature.attribute("Origin"))
        origin.setValue(0, 0, 0)
        dirx = geomDataAPI_Dir(aSketchFeature.attribute("DirX"))
        dirx.setValue(1, 0, 0)
        norm = geomDataAPI_Dir(aSketchFeature.attribute("Norm"))
        norm.setValue(0, 0, 1)
        aSketchCircle = aSketchFeature.addFeature("SketchCircle")
        anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
        aCircleRadius = aSketchCircle.real("circle_radius")
        anCircleCentr.setValue(0.5 + step * (0.5 + i), 0.5 + step * (0.5 + j))
        aCircleRadius.setValue(radius)
        aSketchFeatures.append(aSketchFeature)

aSession.finishOperation()

#=========================================================================
# Make extrusions on circles
#=========================================================================
# Build shape from sketcher results

# Create extrusions
aSession.startOperation()

anExtrusions = []
for i in range(0, N * N):
    aSketchResult = modelAPI_ResultConstruction(aSketchFeatures[i].firstResult())
    anExtrusionFt = aPart.addFeature("Extrusion")
    assert (anExtrusionFt.getKind() == "Extrusion")

    anExtrusionFt.selectionList("base").append(
        aSketchResult, aSketchResult.face(0))
    anExtrusionFt.string("CreationMethod").setValue("BySizes")
    anExtrusionFt.real("from_size").setValue(0)
    anExtrusionFt.real("to_size").setValue(10)
    anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
    anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
    anExtrusions.append(anExtrusionFt)

aSession.finishOperation()

#=========================================================================
# Make rectangle sketch: base for the box, size 100x100
#=========================================================================
aSession.startOperation()
aQuadrangleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aQuadrangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aQuadrangleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLineA = aQuadrangleSketchFeature.addFeature("SketchLine")
aSketchLineB = aQuadrangleSketchFeature.addFeature("SketchLine")
aSketchLineC = aQuadrangleSketchFeature.addFeature("SketchLine")
aSketchLineD = aQuadrangleSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineDStartPoint = geomDataAPI_Point2D(aSketchLineD.attribute("StartPoint"))
aLineDEndPoint = geomDataAPI_Point2D(aSketchLineD.attribute("EndPoint"))
aLineAStartPoint.setValue(0., 0.)
aLineAEndPoint.setValue(0., 100.)
aLineBStartPoint.setValue(0., 100.)
aLineBEndPoint.setValue(100., 100.)
aLineCStartPoint.setValue(100., 100.)
aLineCEndPoint.setValue(100., 0.)
aLineDStartPoint.setValue(100., 0.)
aLineDEndPoint.setValue(0., 0.)

aSession.finishOperation()
aSession.startOperation()

#=========================================================================
# Build a big box extrusion
#=========================================================================
aSketchResult = modelAPI_ResultConstruction(aQuadrangleSketchFeature.firstResult())
# Create extrusion on them
aBox = aPart.addFeature("Extrusion")
aBox.selectionList("base").append(
    aSketchResult, aSketchResult.face(0))
aBox.string("CreationMethod").setValue("BySizes")
aBox.real("from_size").setValue(0)
aBox.real("to_size").setValue(10)
aBox.real("to_offset").setValue(0) #TODO: remove
aBox.real("from_offset").setValue(0) #TODO: remove

aSession.finishOperation()


#=========================================================================
# Create a boolean cut of cylinders from the box:
#=========================================================================
aCurrentResult = modelAPI_ResultBody(aBox.firstResult())
aSession.startOperation()

aBooleanFt = aPart.addFeature("Cut")
aBooleanFt.selectionList("main_objects").append(aCurrentResult, aCurrentResult.shape())
for i in range(0, N * N):
    anExtrusionResult = modelAPI_ResultBody(anExtrusions[i].firstResult())
    aBooleanFt.selectionList("tool_objects").append(anExtrusionResult, anExtrusionResult.shape())
aBooleanFt.execute()
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(aBooleanFt))
assert (len(aBooleanFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(aBooleanFt.firstResult())
assert (aCurrentResult is not None)
aSession.finishOperation()

#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())

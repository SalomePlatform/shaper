# Copyright (C) 2014-2024  CEA, EDF
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
      TestRevolution.py
      Unit test of FeaturesPlugin_Revolution class

      class FeaturesPlugin_Revolution : public ModelAPI_Feature
        static const std::string MY_REVOLUTION_ID("Revolution");
        static const std::string MY_GROUP_LIST_ID("base");
        static const std::string MY_TO_OBJECT_ID("axis_object");
        static const std::string METHOD_ATTR("CreationMethod");
        static const std::string MY_TO_ANGLE_ID("to_angle");
        static const std::string MY_FROM_ANGLE_ID("from_angle");
        static const std::string MY_TO_OBJECT_ID("to_object");
        static const std::string MY_TO_OFFSET_ID("to_offset");
        static const std::string MY_FROM_OBJECT_ID("from_object");
        static const std::string MY_FROM_OFFSET_ID("from_offset");

        data()->addAttribute(LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
        data()->addAttribute(AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
        data()->addAttribute(TO_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()
# Create a part for revol
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
assert (len(aPartFeature.results()) == 1)
# Another way is:
# aPart = aSession.activeDocument()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

#=========================================================================
# Create a sketch circle to revol
#=========================================================================
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

# Create circle
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(0., 0.)
aCircleRadius.setValue(30.)
aSession.finishOperation()

# Build shape from sketcher results
aCircleSketchResult = modelAPI_ResultConstruction(aCircleSketchFeature.firstResult())
assert (aCircleSketchResult.facesNum() > 0)

#=========================================================================
# Create a sketch line to revol
#=========================================================================
aSession.startOperation()
aLineSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aLineSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aLineSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aLineSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)

aSketchLine = aLineSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(-100., -100.)
aLineEndPoint.setValue(100., -100.)
aSession.finishOperation()

# Build shape from sketcher results
aLineSketchResult = aLineSketchFeature.firstResult()
aLineSketchShape = modelAPI_ResultConstruction(aLineSketchResult).shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aLineSketchShape, GeomAPI_Shape.EDGE)
aLineEdge = aShapeExplorer.current()

#=========================================================================
# Test revol between from and to angles
#=========================================================================
aSession.startOperation()
aRevolFt = aPart.addFeature("Revolution")
assert (aRevolFt.getKind() == "Revolution")
# selection type FACE=4
aRevolFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchResult.face(0))
aRevolFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
aRevolFt.string("CreationMethod").setValue("ByAngles")
aRevolFt.real("from_angle").setValue(10)
aRevolFt.real("to_angle").setValue(10)
aRevolFt.real("to_offset").setValue(0) #TODO: remove
aRevolFt.real("from_offset").setValue(0) #TODO: remove
aRevolFt.execute()
aSession.finishOperation()
assert (aRevolFt.real("from_angle").value() == 10.0)
assert (aRevolFt.real("to_angle").value() == 10.0)

# Check revol results
assert (len(aRevolFt.results()) > 0)
aRevolResult = modelAPI_ResultBody(aRevolFt.firstResult())
assert (aRevolResult is not None)

#=========================================================================
# Create bounding planes
#=========================================================================
# Create from plane
aSession.startOperation()
aFromPlaneFeature = aPart.addFeature("Plane")
aFromPlaneFeature.string("creation_method").setValue("by_general_equation")
aFromPlaneFeature.string("by_other_plane_option").setValue("by_distance_from_other") # TODO: remove
aFromPlaneFeature.real("A").setValue(0.)
aFromPlaneFeature.real("B").setValue(0.)
aFromPlaneFeature.real("C").setValue(1.)
aFromPlaneFeature.real("D").setValue(50.)
aSession.finishOperation()
aFromResult = aFromPlaneFeature.firstResult()
aFromShape = modelAPI_ResultConstruction(aFromResult).shape()

# Create to plane
aSession.startOperation()
aToPlaneFeature = aPart.addFeature("Plane")
aToPlaneFeature.string("creation_method").setValue("by_general_equation")
aToPlaneFeature.string("by_other_plane_option").setValue("by_distance_from_other") # TODO: remove
aToPlaneFeature.real("A").setValue(0.)
aToPlaneFeature.real("B").setValue(0.)
aToPlaneFeature.real("C").setValue(1.)
aToPlaneFeature.real("D").setValue(-50.)
aSession.finishOperation()
aToResult = aToPlaneFeature.firstResult()
aToShape = modelAPI_ResultConstruction(aToResult).shape()

#=========================================================================
# Test revol between bounding planes
#=========================================================================
aSession.startOperation()
aRevolFt = aPart.addFeature("Revolution")
assert (aRevolFt.getKind() == "Revolution")
# selection type FACE=4
aRevolFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchResult.face(0))
aRevolFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
aRevolFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
aRevolFt.real("from_angle").setValue(0) #TODO: remove
aRevolFt.real("to_angle").setValue(0) #TODO: remove
aRevolFt.selection("to_object").setValue(aToResult, None)
aRevolFt.real("to_offset").setValue(0)
aRevolFt.selection("from_object").setValue(aFromResult, None)
aRevolFt.real("from_offset").setValue(0)
aRevolFt.execute()
aSession.finishOperation()

# Check revol results
assert (len(aRevolFt.results()) > 0)
aRevolResult = modelAPI_ResultBody(aRevolFt.firstResult())
assert (aRevolResult is not None)
aSession.undo()

#=========================================================================
# Test revol between bounding plane
#=========================================================================
aSession.startOperation()
aRevolFt = aPart.addFeature("Revolution")
assert (aRevolFt.getKind() == "Revolution")
# selection type FACE=4
aRevolFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchResult.face(0))
aRevolFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
aRevolFt.string("CreationMethod").setValue("ByPlanesAndOffsets")
aRevolFt.real("from_angle").setValue(0) #TODO: remove
aRevolFt.real("to_angle").setValue(0) #TODO: remove
aRevolFt.selection("to_object").setValue(aToResult, None)
aRevolFt.real("to_offset").setValue(0)
aRevolFt.selection("from_object").setValue(None, None)
aRevolFt.real("from_offset").setValue(0)
aRevolFt.execute()
aSession.finishOperation()

# Check revol results
assert (len(aRevolFt.results()) > 0)
aRevolResult = modelAPI_ResultBody(aRevolFt.firstResult())
assert (aRevolResult is not None)

from salome.shaper import model
assert(model.checkPythonDump())

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
      TestRevolutionFuse.py
      Unit test of FeaturesPlugin_RevolutionFuse class

      class FeaturesPlugin_RevolutionFuse : public FeaturesPlugin_RevolutionBoolean
		static const std::string MY_REVOLUTION_ID("RevolutionFuse");
        static const std::string MY_SKETCH_OBJECT_ID("sketch");
        static const std::string MY_AXIS_ID("axis_object");
        static const std::string METHOD_ATTR("CreationMethod");
        static const std::string MY_TO_ANGLE_ID("to_angle");
        static const std::string MY_FROM_ANGLE_ID("from_angle");
        static const std::string MY_TO_OBJECT_ID("to_object");
        static const std::string MY_TO_OFFSET_ID("to_offset");
        static const std::string MY_FROM_OBJECT_ID("from_object");
        static const std::string MY_FROM_OFFSET_ID("from_offset");
        static const std::string MY_BOOLEAN_OBJECTS_ID("boolean_objects");

        data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());
        data()->addAttribute(AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
		data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());
        data()->addAttribute(TO_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());
        data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeSelection::typeId());
        data()->addAttribute(BOOLEAN_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *
import math

__updated__ = "2014-12-16"

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
# Create a sketch circle to extrude
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
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(20)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aCircleSketchResult = modelAPI_ResultConstruction(aCircleSketchFeature.firstResult())
assert (aCircleSketchResult.facesNum() > 0)
# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(
    aCircleSketchResult, aCircleSketchResult.face(0))
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(0)
anExtrusionFt.real("to_offset").setValue(0) #TODO: remove
anExtrusionFt.real("from_offset").setValue(0) #TODO: remove
anExtrusionFt.execute()
aSession.finishOperation()
assert (anExtrusionFt.real("to_size").value() == 50.0)

# Check extrusion results
assert (len(anExtrusionFt.results()) > 0)
anExtrusionResult = modelAPI_ResultBody(anExtrusionFt.firstResult())
assert (anExtrusionResult is not None)

#=========================================================================
# Create a sketch line to revol
#=========================================================================
aSession.startOperation()
aLineSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aLineSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 50)
dirx = geomDataAPI_Dir(aLineSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aLineSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aLineSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/TopFace")

aSketchLine = aLineSketchFeature.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aSketchLine.attribute("StartPoint"))
aLineEndPoint = geomDataAPI_Point2D(aSketchLine.attribute("EndPoint"))
aLineStartPoint.setValue(-20, -20)
aLineEndPoint.setValue(20, -20)
aSession.finishOperation()

# Build shape from sketcher results
aLineSketchResult = aLineSketchFeature.firstResult()
aLineSketchShape = modelAPI_ResultConstruction(aLineSketchResult).shape()
aShapeExplorer = GeomAPI_ShapeExplorer(aLineSketchShape, GeomAPI_Shape.EDGE)
aLineEdge = aShapeExplorer.current()

#=========================================================================
# Make revolution fuse
#=========================================================================
aSession.startOperation()
anRevolutionFuseFt = featureToCompositeFeature(aPart.addFeature("RevolutionFuse"))
assert (anRevolutionFuseFt.getKind() == "RevolutionFuse")
# selection type FACE=4
aSession.startOperation()
aCircleSketchFeature = featureToCompositeFeature(anRevolutionFuseFt.addFeature("Sketch"))
origin = geomDataAPI_Point(aCircleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 50)
dirx = geomDataAPI_Dir(aCircleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aCircleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aCircleSketchFeature.selection("External").selectSubShape("face", "Extrusion_1/To_Face")
aSession.startOperation()
aSketchCircle = aCircleSketchFeature.addFeature("SketchCircle")
anCircleCentr = geomDataAPI_Point2D(aSketchCircle.attribute("circle_center"))
aCircleRadius = aSketchCircle.real("circle_radius")
anCircleCentr.setValue(0, 0)
aCircleRadius.setValue(10)
aSession.finishOperation()
aSession.finishOperation()
aSession.startOperation()
aCircleSketchFeature.execute() # execute for sketch should be called here, because it is not set as current feature, so it is disabled.
anRevolutionFuseFt.selectionList("base").append(aCircleSketchFeature.firstResult(), None)
anRevolutionFuseFt.selection("axis_object").setValue(aLineSketchResult, aLineEdge)
anRevolutionFuseFt.string("CreationMethod").setValue("ByAngles")
anRevolutionFuseFt.real("to_angle").setValue(50)
anRevolutionFuseFt.real("from_angle").setValue(50)
anRevolutionFuseFt.real("to_offset").setValue(0) #TODO: remove
anRevolutionFuseFt.real("from_offset").setValue(0) #TODO: remove
anRevolutionFuseFt.selectionList("main_objects").append(anExtrusionResult, anExtrusionResult.shape())
aSession.finishOperation()
aSession.finishOperation()

#=========================================================================
# Test results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert (aFactory.validate(anRevolutionFuseFt))
assert (len(anRevolutionFuseFt.results()) > 0)
aCurrentResult = modelAPI_ResultBody(anRevolutionFuseFt.firstResult())
assert (aCurrentResult is not None)

from salome.shaper import model
assert(model.checkPythonDump())

## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

"""
      TestBoolean.py
      Unit test of FeaturesPlugin_Group class

      class FeaturesPlugin_Group
        static const std::string MY_GROUP_ID("Group");
        static const std::string MY_GROUP_LIST_ID("group_list");

        data()->addAttribute(FeaturesPlugin_Group::LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
"""
#=========================================================================
# Initialization of the test
#=========================================================================
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

__updated__ = "2014-12-16"

aSession = ModelAPI_Session.get()
# Create a part for extrusions & boolean
aSession.startOperation()
aPartFeature = aSession.moduleDocument().addFeature("Part")
aSession.finishOperation()
aPart = aSession.activeDocument()
#=========================================================================
# Create a sketch with triangle and extrude it
#=========================================================================
aSession.startOperation()
aTriangleSketchFeature = featureToCompositeFeature(aPart.addFeature("Sketch"))
origin = geomDataAPI_Point(aTriangleSketchFeature.attribute("Origin"))
origin.setValue(0, 0, 0)
dirx = geomDataAPI_Dir(aTriangleSketchFeature.attribute("DirX"))
dirx.setValue(1, 0, 0)
norm = geomDataAPI_Dir(aTriangleSketchFeature.attribute("Norm"))
norm.setValue(0, 0, 1)
aSketchLineA = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineB = aTriangleSketchFeature.addFeature("SketchLine")
aSketchLineC = aTriangleSketchFeature.addFeature("SketchLine")
aLineAStartPoint = geomDataAPI_Point2D(aSketchLineA.attribute("StartPoint"))
aLineAEndPoint = geomDataAPI_Point2D(aSketchLineA.attribute("EndPoint"))
aLineBStartPoint = geomDataAPI_Point2D(aSketchLineB.attribute("StartPoint"))
aLineBEndPoint = geomDataAPI_Point2D(aSketchLineB.attribute("EndPoint"))
aLineCStartPoint = geomDataAPI_Point2D(aSketchLineC.attribute("StartPoint"))
aLineCEndPoint = geomDataAPI_Point2D(aSketchLineC.attribute("EndPoint"))
aLineAStartPoint.setValue(-100., 0.)
aLineAEndPoint.setValue(100., 0.)
aLineBStartPoint.setValue(100., 0.)
aLineBEndPoint.setValue(0., 173.2)
aLineCStartPoint.setValue(0., 173.2)
aLineCEndPoint.setValue(-100., 0.)
aSession.finishOperation()
# Build sketch faces
aSession.startOperation()
aSketchResult = modelAPI_ResultConstruction(aTriangleSketchFeature.firstResult())
# Create extrusion on them
anExtrusionFt = aPart.addFeature("Extrusion")
anExtrusionFt.selectionList("base").append(aSketchResult, aSketchResult.face(0))
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(50)
anExtrusionFt.execute()
aSession.finishOperation()
anExtrusionBody = modelAPI_ResultBody(anExtrusionFt.firstResult())
#=========================================================================
# Create group of vertex
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("vertex")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/From_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/From_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/From_Face]")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 6)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# Create group of edges
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("edge")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/From_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/From_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/From_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/To_Face]")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 9)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# Create group of faces
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("face")
aShapeExplorer = GeomAPI_ShapeExplorer(anExtrusionBody.shape(), GeomAPI_Shape.FACE)
while aShapeExplorer.more():
  aSelectionListAttr.append(anExtrusionBody, aShapeExplorer.current())
  aShapeExplorer.next();
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 5)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
#=========================================================================
# Create group of solids
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append(anExtrusionBody, None)
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 1)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)

#=========================================================================
# Create group of face
#=========================================================================
aSession.startOperation()
aGroupFeature = aSession.activeDocument().addFeature("Group")
aSelectionListAttr = aGroupFeature.selectionList("group_list")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append("Extrusion_1_1/To_Face")
aSession.finishOperation()
#=========================================================================
# Check results
#=========================================================================
assert(aSelectionListAttr.size() == 1)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)

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
anCircleCentr.setValue(0., 57.74)
aCircleRadius.setValue(50.)
aSession.finishOperation()
#=========================================================================
# Make extrusion on circle
#=========================================================================
# Build shape from sketcher results
aCircleSketchResult = modelAPI_ResultConstruction(aCircleSketchFeature.firstResult())
assert(aCircleSketchResult.facesNum() > 0)
# Create extrusion
aSession.startOperation()
anExtrusionFt = aPart.addFeature("Extrusion")
assert (anExtrusionFt.getKind() == "Extrusion")
# selection type FACE=4
anExtrusionFt.selectionList("base").append(aCircleSketchResult, aCircleSketchResult.face(0))
anExtrusionFt.string("CreationMethod").setValue("BySizes")
anExtrusionFt.real("to_size").setValue(50)
anExtrusionFt.real("from_size").setValue(50)
anExtrusionFt.execute()
aSession.finishOperation()
aCylinderBody = modelAPI_ResultBody(anExtrusionFt.firstResult())

#=========================================================================
# Create a cut
#=========================================================================
aSession.startOperation()
aBooleanFt = aPart.addFeature("Cut")
aBooleanFt.selectionList("main_objects").append(anExtrusionBody, None)
aBooleanFt.selectionList("tool_objects").append(aCylinderBody, None)
aBooleanFt.execute()
aSession.finishOperation()

#=========================================================================
# Move group feature
#=========================================================================
aSession.startOperation()
aPart.moveFeature(aGroupFeature, aBooleanFt)
aSession.finishOperation()

#=========================================================================
# Check results
#=========================================================================
aFactory = ModelAPI_Session.get().validators()
assert(aFactory.validate(aGroupFeature))
assert(aSelectionListAttr.size() == 1)
assert(len(aGroupFeature.results()) > 0)
aGroupResult = aGroupFeature.firstResult()
assert(aGroupResult)
assert(aGroupResult.shape())
#=========================================================================
# End of test
#=========================================================================

from salome.shaper import model
assert(model.checkPythonDump())

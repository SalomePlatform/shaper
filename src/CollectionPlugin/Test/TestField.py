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
anExtrusionFt.real("to_offset").setValue(0)
anExtrusionFt.real("from_offset").setValue(0)
anExtrusionFt.execute()
aSession.finishOperation()
anExtrusionBody = modelAPI_ResultBody(anExtrusionFt.firstResult())
#=========================================================================
# Create doubles field on vertices
#=========================================================================
aSession.startOperation()
aField = aSession.activeDocument().addFeature("Field")
aSelectionListAttr = aField.selectionList("selected")
aSelectionListAttr.setSelectionType("vertex")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_3][Extrusion_1_1/To_Face]")
aSelectionListAttr.append("[Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_2][Extrusion_1_1/From_Face]")
aComponentNames = aField.stringArray("components_names")
aComponentNames.setSize(2) # two components
aComponentNames.setValue(0, "temperatue")
aComponentNames.setValue(1, "porosity")
aStamps = aField.intArray("stamps")
aStamps.setSize(1) # one step
aStamps.setValue(0, 10)
aTables = aField.tables("values")
aTables.setType(2) # double
aTables.setSize(1 + 3, 2, 1) # default row + number of selected, number of compoents, number of steps (tables)
aTables.setValue(20, 0, 0, 0) # value, index of selection, index of component, index of step
aTables.setValue(35, 1, 0, 0)
aTables.setValue(27, 2, 0, 0)
aTables.setValue(28, 3, 0, 0)
aTables.setValue(0.5, 0, 1, 0)
aTables.setValue(0.55, 1, 1, 0)
aTables.setValue(0.39, 2, 1, 0)
aTables.setValue(0.40, 3, 1, 0)
aSession.finishOperation()
#=========================================================================
# Create strings field on faces
#=========================================================================
aSession.startOperation()
aField = aSession.activeDocument().addFeature("Field")
aSelectionListAttr = aField.selectionList("selected")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1")
aComponentNames = aField.stringArray("components_names")
aComponentNames.setSize(1) # one component
aComponentNames.setValue(0, "description")
aStamps = aField.intArray("stamps")
aStamps.setSize(2) # two steps
aStamps.setValue(0, 1)
aStamps.setValue(1, 3)
aTables = aField.tables("values")
aTables.setType(3) # string
aTables.setSize(1 + 1, 1, 2) # default row + number of selected, number of compoents, number of steps (tables)
aTables.setValue("-default-", 0, 0, 0) # value, index of selection, index of component, index of step
aTables.setValue("-default-", 0, 0, 1)
aTables.setValue("Face one", 1, 0, 0)
aTables.setValue("Face two", 1, 0, 1)
aSession.finishOperation()

aFieldResult = aField.firstResult()
assert(aFieldResult)
#=========================================================================
# Create integer field on faces
#=========================================================================
aSession.startOperation()
aField = aSession.activeDocument().addFeature("Field")
aSelectionListAttr = aField.selectionList("selected")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1")
aComponentNames = aField.stringArray("components_names")
aComponentNames.setSize(1) # one component
aComponentNames.setValue(0, "description")
aStamps = aField.intArray("stamps")
aStamps.setSize(1) # one step
aStamps.setValue(0, 0)
aTables = aField.tables("values")
aTables.setType(1) # integer
aTables.setSize(1 + 1, 1, 1) # default row + number of selected, number of compoents, number of steps (tables)
aTables.setValue(0, 0, 0, 0) # value, index of selection, index of component, index of step
aTables.setValue(2, 1, 0, 0)
aSession.finishOperation()

aFieldResult = aField.firstResult()
assert(aFieldResult)

#=========================================================================
# Create Boolean field on faces
#=========================================================================
aSession.startOperation()
aField = aSession.activeDocument().addFeature("Field")
aSelectionListAttr = aField.selectionList("selected")
aSelectionListAttr.setSelectionType("face")
aSelectionListAttr.append("Extrusion_1_1/Generated_Face&Sketch_1/SketchLine_1")
aComponentNames = aField.stringArray("components_names")
aComponentNames.setSize(1) # one component
aComponentNames.setValue(0, "description")
aStamps = aField.intArray("stamps")
aStamps.setSize(1) # one step
aStamps.setValue(0, 0)
aTables = aField.tables("values")
aTables.setType(0) # boolean
aTables.setSize(1 + 1, 1, 1) # default row + number of selected, number of compoents, number of steps (tables)
aTables.setValue(True, 0, 0, 0) # value, index of selection, index of component, index of step
aTables.setValue(False, 1, 0, 0)
aSession.finishOperation()

aFieldResult = aField.firstResult()
assert(aFieldResult)

from salome.shaper import model
assert(model.checkPythonDump())

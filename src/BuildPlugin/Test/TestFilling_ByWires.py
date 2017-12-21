## Copyright (C) 2017-20xx  CEA/DEN, EDF R&D
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

# Initialization of the test
from ModelAPI import *
from GeomDataAPI import *
from GeomAlgoAPI import *
from GeomAPI import *

# Get document
aSession = ModelAPI_Session.get()
aDocument = aSession.moduleDocument()

# Create a part
aSession.startOperation()
aPartFeature = aDocument.addFeature("Part")
aSession.finishOperation()
aPartResult = modelAPI_ResultPart(aPartFeature.firstResult())
aPart = aPartResult.partDoc()

# Create first wire in a sketch
aSession.startOperation()
aSketch1 = featureToCompositeFeature(aPart.addFeature("Sketch"))
anOrigin = geomDataAPI_Point(aSketch1.attribute("Origin"))
anOrigin.setValue(0, 0, 0)
aDirX = geomDataAPI_Dir(aSketch1.attribute("DirX"))
aDirX.setValue(1, 0, 0)
aNorm = geomDataAPI_Dir(aSketch1.attribute("Norm"))
aNorm.setValue(0, 0, 1)
# arc 1
anArc1 = aSketch1.addFeature("SketchArc")
anArcCenter = geomDataAPI_Point2D(anArc1.attribute("center_point"))
anArcCenter.setValue(1, 1)
anArcStartPoint = geomDataAPI_Point2D(anArc1.attribute("start_point"))
anArcStartPoint.setValue(20, 5)
anArcEndPoint = geomDataAPI_Point2D(anArc1.attribute("end_point"))
anArcEndPoint.setValue(5, 20)
# arc 2
anArc2 = aSketch1.addFeature("SketchArc")
anArcCenter = geomDataAPI_Point2D(anArc2.attribute("center_point"))
anArcCenter.setValue(12.5, 12.5)
anArcStartPoint = geomDataAPI_Point2D(anArc2.attribute("start_point"))
anArcStartPoint.setValue(5, 20)
anArcEndPoint = geomDataAPI_Point2D(anArc2.attribute("end_point"))
anArcEndPoint.setValue(20, 5)
aSession.finishOperation()
aSketch1Result = aSketch1.firstResult()
# a wire
aSession.startOperation()
aWire1 = aPart.addFeature("Wire")
aBaseObjectsList = aWire1.selectionList("base_objects")
aBaseObjectsList.append(aSketch1Result, anArc1.lastResult().shape())
aBaseObjectsList.append(aSketch1Result, anArc2.lastResult().shape())
aSession.finishOperation()

# Create second wire in another sketch
aSession.startOperation()
aSketch2 = featureToCompositeFeature(aPart.addFeature("Sketch"))
anOrigin = geomDataAPI_Point(aSketch2.attribute("Origin"))
anOrigin.setValue(0, 0, 0)
aDirX = geomDataAPI_Dir(aSketch2.attribute("DirX"))
aDirX.setValue(1, 0, 0)
aNorm = geomDataAPI_Dir(aSketch2.attribute("Norm"))
aNorm.setValue(0, 0.7071067811865475, 0.7071067811865475)
# line 1
aLine1 = aSketch2.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aLine1.attribute("StartPoint"))
aLineStartPoint.setValue(30, 0)
aLineEndPoint = geomDataAPI_Point2D(aLine1.attribute("EndPoint"))
aLineEndPoint.setValue(20, 20)
# line 2
aLine2 = aSketch2.addFeature("SketchLine")
aLineStartPoint = geomDataAPI_Point2D(aLine2.attribute("StartPoint"))
aLineStartPoint.setValue(20, 20)
aLineEndPoint = geomDataAPI_Point2D(aLine2.attribute("EndPoint"))
aLineEndPoint.setValue(20, 0)
aSession.finishOperation()
aSketch2Result = aSketch2.firstResult()
# a wire
aSession.startOperation()
aWire2 = aPart.addFeature("Wire")
aBaseObjectsList = aWire2.selectionList("base_objects")
aBaseObjectsList.append(aSketch2Result, aLine1.lastResult().shape())
aBaseObjectsList.append(aSketch2Result, aLine2.lastResult().shape())
aSession.finishOperation()

# Create filling
aSession.startOperation()
aFillingFeature = aPart.addFeature("Filling")
aBaseObjectsList = aFillingFeature.selectionList("base_objects")
aBaseObjectsList.append(aWire1.lastResult(), None)
aSession.finishOperation()

# =============================================================================
# Test 1. Filling on one wire is failed (no result is built)
# =============================================================================
assert(len(aFillingFeature.results()) == 0)

# =============================================================================
# Test 2. Add another wire, filling should be completed
# =============================================================================
aSession.startOperation()
aBaseObjectsList.append(aWire2.lastResult(), None)
aSession.finishOperation()
assert(len(aFillingFeature.results()) > 0)

# =============================================================================
# Test 3. Change parameters one-by-one and check validity of result
# =============================================================================
aSession.startOperation()
aFillingFeature.string("advanced_options").setValue("true")
aSession.finishOperation()
orientations = ["auto_correct", "curve_info", "edge_orient"]
tolerances = [0.0001, 0.001]
for ori in orientations:
  for minDeg in range(2, 4):
    for maxDeg in range(5, 7):
      for nbIter in range(0, 3, 2):
        for tol2d in tolerances:
          for tol3d in tolerances:
            for approx in [False, True]:
              aSession.startOperation()
              aFillingFeature.string("orientation").setValue(ori)
              aFillingFeature.integer("min_degree").setValue(minDeg)
              aFillingFeature.integer("max_degree").setValue(maxDeg)
              aFillingFeature.integer("nb_iter").setValue(nbIter)
              aFillingFeature.real("tol_2d").setValue(tol2d)
              aFillingFeature.real("tol_3d").setValue(tol3d)
              aFillingFeature.boolean("approximation").setValue(approx)
              aSession.finishOperation()
              assert(len(aFillingFeature.results()) > 0), "Filling feature failed with parameters:\n  orientation={}\n  min deg={}\n  max deg={}\n  nb iter={}\n  tol 2d={}\n  tol 3d={}\n  approximation={}".format(ori, minDeg, maxDeg, nbIter, tol2d, tol3d, approx)

from salome.shaper import model
assert(model.checkPythonDump())

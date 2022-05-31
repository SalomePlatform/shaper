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

from salome.shaper import model

from ModelAPI import *
from GeomDataAPI import *
from ModelGeomAlgo import ModelGeomAlgo_Point2D
from salome.shaper import geom
import math

TOLERANCE = 1.e-7

SketchLineId = 'SketchLine'
SketchArcId = 'SketchArc'
SketchConstraintCoincidenceId = 'SketchConstraintCoincidence'
SketchConstraintEqualId = 'SketchConstraintEqual'

aSession = ModelAPI_Session.get()
model.begin()
partSet = model.moduleDocument()
Part = model.addPart(partSet)
Part_doc = Part.document()

# Test1:begin split on arc with coincident point and intersection line : smaller part
Sketch = model.addSketch(Part_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch.addArc(50, 50, 30, 50, 50, 70, False)
SketchArc_2 = Sketch.addArc(75, 80, 50, 70, 100, 100, True)
SketchLine = Sketch.addLine(60, 31, 60, 130)
SketchLine_2 = Sketch.addLine(10, 40, 90, 40)

# correct start point of SketchLine to belong the arc for trim
Intersected_Points = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc_1.feature(), FeatureList([SketchLine.feature()]))
assert(len(Intersected_Points) == 2)
if Intersected_Points[0].y() < Intersected_Points[1].y():
  IntersectionPoint = Intersected_Points[0]
else:
  IntersectionPoint = Intersected_Points[1]

SketchLine.startPoint().setValue(IntersectionPoint.x(), IntersectionPoint.y())

SketchConstraintCoincidence_1_1 = Sketch.setCoincident(SketchArc_1.endPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_1_2 = Sketch.setCoincident(SketchArc_1.results()[1], SketchLine.startPoint())
model.do()


#find point on arc, where to perform trim
SketchLine_intersecting = Sketch.addLine(50, 50, 100, 0)
Geom_Points = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchArc_1.feature(), FeatureList([SketchLine_intersecting.feature()]))
assert(len(Geom_Points) == 1)
ModelAPI.removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))
model.do()
GeomPoint = Geom_Points[0]

model.do()
SketchTrim = Sketch.addTrim(SketchArc_1, Sketch.to2D(GeomPoint))
SketchTrim.execute()

model.end()

assert(model.checkPythonDump())

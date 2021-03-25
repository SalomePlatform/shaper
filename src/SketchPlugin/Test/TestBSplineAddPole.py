# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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
    Test for adding a pole to already created B-spline curve
"""

from salome.shaper import model
from GeomAPI import *
import random

TOLERANCE = 1.e-7

def assertSubFeatures(theSketch, theNbPoints, theNbLines, theNbSplines, theNbSplinesP, theNbCoincidences, theNbInternal):
    model.testNbSubFeatures(theSketch, "SketchPoint", theNbPoints)
    model.testNbSubFeatures(theSketch, "SketchLine", theNbLines)
    model.testNbSubFeatures(theSketch, "SketchBSpline", theNbSplines)
    model.testNbSubFeatures(theSketch, "SketchBSplinePeriodic", theNbSplinesP)
    model.testNbSubFeatures(theSketch, "SketchConstraintCoincidence", theNbCoincidences)
    model.testNbSubFeatures(theSketch, "SketchConstraintCoincidenceInternal", theNbInternal)

def assertPoles(thePoles, theReference):
    assert(thePoles.size() == len(theReference))
    for ind in range(0, len(theReference)):
        pole = thePoles.pnt(ind)
        ref = GeomAPI_Pnt2d(theReference[ind][0], theReference[ind][1])
        assert(model.distancePointPoint(pole, ref) < TOLERANCE), "Index = {}, pole = ({}, {}), refdata = ({}, {})".format(ind, pole.x(), pole.y(), ref.x(), ref.y())


model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

SketchBSpline_1_poles = [(-25, 5), (-15, 35), (15, 35), (28, 5)]
SketchBSpline_1 = Sketch_1.addSpline(poles = SketchBSpline_1_poles)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4] = SketchBSpline_1.controlPoles(auxiliary = [0, 1, 2, 3])
[SketchLine_1, SketchLine_2, SketchLine_3] = SketchBSpline_1.controlPolygon(auxiliary = [0, 1, 2])

SketchBSplinePeriodic_1_poles = [(-20, -10), (20, -40), (20, -10), (-20, -40)]
SketchBSplinePeriodic_1 = Sketch_1.addSpline(poles = SketchBSplinePeriodic_1_poles, periodic = True)
[SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchPoint_8] = SketchBSplinePeriodic_1.controlPoles(auxiliary = [0, 1, 2, 3])
[SketchLine_4, SketchLine_5, SketchLine_6, SketchLine_7] = SketchBSplinePeriodic_1.controlPolygon(auxiliary = [0, 1, 2, 3])
model.do()

# check original values
NBPOINTS = 8
NBLINES = 7
NBSPLINES = 1
NBSPLINESPERIODIC = 1
NBCOINCIDENCES = 0
NBINTERNAL = 22
assertSubFeatures(Sketch_1, NBPOINTS, NBLINES, NBSPLINES, NBSPLINESPERIODIC, NBCOINCIDENCES, NBINTERNAL)
assertPoles(SketchBSpline_1.poles(), SketchBSpline_1_poles)
assertPoles(SketchBSplinePeriodic_1.poles(), SketchBSplinePeriodic_1_poles)

# add poles to non-periodic B-spline
ind = 0
while ind < len(SketchBSpline_1_poles):
    x = random.uniform(-25, 25)
    y = random.uniform(5, 40)
    SketchBSpline_1.insertPole(ind, GeomAPI_Pnt2d(x, y))
    if ind + 1 < len(SketchBSpline_1_poles):
        SketchBSpline_1_poles.insert(ind + 1, (x, y))
    else:
        SketchBSpline_1_poles.insert(len(SketchBSpline_1_poles) - 1, (x, y))
    ind += 2
    model.do()

    NBPOINTS += 1
    NBLINES += 1
    NBINTERNAL += 3
    assertSubFeatures(Sketch_1, NBPOINTS, NBLINES, NBSPLINES, NBSPLINESPERIODIC, NBCOINCIDENCES, NBINTERNAL)
    assertPoles(SketchBSpline_1.poles(), SketchBSpline_1_poles)

# add poles to periodic B-spline
ind = 0
while ind < len(SketchBSplinePeriodic_1_poles):
    x = random.uniform(-25, 25)
    y = random.uniform(-45, -5)
    SketchBSplinePeriodic_1.insertPole(ind, GeomAPI_Pnt2d(x, y))
    SketchBSplinePeriodic_1_poles.insert(ind + 1, (x, y))
    ind += 2
    model.do()

    NBPOINTS += 1
    NBLINES += 1
    NBINTERNAL += 3
    assertSubFeatures(Sketch_1, NBPOINTS, NBLINES, NBSPLINES, NBSPLINESPERIODIC, NBCOINCIDENCES, NBINTERNAL)
    assertPoles(SketchBSplinePeriodic_1.poles(), SketchBSplinePeriodic_1_poles)

model.end()

# check error on incorrect action
model.begin()
assert(not SketchBSpline_1.feature().customAction("wrong_action"))
model.end()

#assert(model.checkPythonDump())

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
    Test2018.py
    Test case for issue #2018 "After split auxiliary circle edit of axiliary state of arc is wrong"
"""

from ModelAPI import *
from ModelAPI import ModelAPI_Feature
from ModelAPI import FeatureList
from ModelGeomAlgo import ModelGeomAlgo_Point2D
from SketchAPI import SketchAPI_Arc
from salome.shaper import model

def getArcs(theSketch):
    anArcs = []
    for aSubObj in theSketch.features().list():
        aFeature = ModelAPI_Feature.feature(aSubObj)
        if aFeature is not None and aFeature.getKind() == "SketchArc":
            anArcs.append(aFeature)
    return anArcs


model.begin()
partSet = model.moduleDocument()
Sketch_1 = model.addSketch(partSet, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-316.2976090885591, -103.0319555403832, 230.3644881376851)
SketchCircle_1.setAuxiliary(True)
SketchLine_1 = Sketch_1.addLine(-489.6131019153831, 48.7229038254136, -457.9545454545454, 334.0909090909091)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchLine_2 = Sketch_1.addLine(-457.9545454545454, 334.0909090909091, -251.2069112222553, 117.9454153513332)
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchCircle_1.results()[1])

# prepare point on circle
SketchLine_intersecting = Sketch_1.addLine(-316.2976090885591, -103.0319555403832, -457.9545454545454, 334.0909090909091)
Intersection_Point = ModelGeomAlgo_Point2D.getSetOfPntIntersectedShape(SketchCircle_1.feature(), FeatureList([SketchLine_intersecting.feature()]))
removeFeaturesAndReferences(FeatureSet([SketchLine_intersecting.feature()]))
GeomPoint = Intersection_Point[0]

#perform split
Sketch_1.addSplit(SketchCircle_1, Sketch_1.to2D(GeomPoint))
model.do()

anArcs = getArcs(Sketch_1)
# check auxiliary flags
for arc in anArcs:
    aCurrentArc = SketchAPI_Arc(arc)
    assert(aCurrentArc.auxiliary().value())
    aCurrentArc.setAuxiliary(False)
    model.do()
    assert(aCurrentArc.auxiliary().value() == False)

model.end()

assert(model.checkPythonDump())

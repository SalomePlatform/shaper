# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

import math

TOLERANCE = 1.e-7

def assertRectangle(theWire):
    assert(theWire is not None)
    aCorners = [GeomAPI.GeomAPI_Pnt(0, 0, 0)]
    assert(theWire.isRectangle(aCorners))

def checkRectangleFace(theDocument, theFaceName):
    aFace = model.addFace(theDocument, [model.selection("FACE", theFaceName)])
    aShape = aFace.result().resultSubShapePair()[0].shape()
    assert(aShape.isFace())
    aSubs = aShape.subShapes(GeomAPI.GeomAPI_Shape.WIRE)
    assert(aSubs.size() == 1)
    assertRectangle(aSubs[0].wire())
    theDocument.removeFeature(aFace.feature())

def checkRectangleWire(theDocument, theEdgeNames):
    aSelection = []
    for name in theEdgeNames:
        aSelection.append(model.selection("EDGE", name))
    aWire = model.addWire(theDocument, aSelection)
    aShape = aWire.result().resultSubShapePair()[0].shape()
    assert(aShape.isWire())
    assert(aShape.wire().isClosed())
    assertRectangle(aShape.wire())
    theDocument.removeFeature(aWire.feature())


def assertPolygon(theWire):
    assert(theWire is not None)
    aCorners = [GeomAPI.GeomAPI_Pnt(0, 0, 0)]
    assert(theWire.isPolygon(aCorners))

def checkPolygonWire(theDocument, theEdgeNames):
    aSelection = []
    for name in theEdgeNames:
        aSelection.append(model.selection("EDGE", name))
    aWire = model.addWire(theDocument, aSelection)
    aShape = aWire.result().resultSubShapePair()[0].shape()
    assert(aShape.isWire())
    assertPolygon(aShape.wire())
    theDocument.removeFeature(aWire.feature())

def checkPolyline(theDocument, theVertexNames):
    aSelection = []
    for name in theVertexNames:
        aSelection.append(model.selection("VERTEX", name))
    aWire = model.addPolyline3D(Part_1_doc, aSelection, False)
    aShape = aWire.result().resultSubShapePair()[0].shape()
    assert(aShape.isWire())
    assertPolygon(aShape.wire())
    theDocument.removeFeature(aWire.feature())



model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.do()

# Test 1. Check face/wire of box is a rectangle
checkRectangleFace(Part_1_doc, "Box_1_1/Top")
Wire_edges = ["[Box_1_1/Left][Box_1_1/Top]", "[Box_1_1/Front][Box_1_1/Top]", "[Box_1_1/Right][Box_1_1/Top]", "[Box_1_1/Back][Box_1_1/Top]"]
checkRectangleWire(Part_1_doc, Wire_edges)

# Test 2. Build a polygon from edges
Wire_edges = ["[Box_1_1/Left][Box_1_1/Bottom]", "[Box_1_1/Front][Box_1_1/Left]", "[Box_1_1/Left][Box_1_1/Top]", "[Box_1_1/Back][Box_1_1/Top]", "[Box_1_1/Right][Box_1_1/Top]"]
checkPolygonWire(Part_1_doc, Wire_edges)

# Test 3. Build a polygon from vertices
Poly_vertices = ["[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]"]
checkPolyline(Part_1_doc, Poly_vertices)

model.end()

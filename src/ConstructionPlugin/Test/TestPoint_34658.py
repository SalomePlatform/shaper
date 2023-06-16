# Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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

import sys
import salome
from ModelAPI import *

def assertPoint(aPoint):
  assert(aPoint is not None)
  aPointFeature = aPoint.feature()
  assert(len(aPointFeature.results()) > 0)
  aPointResult = modelAPI_ResultConstruction(aPointFeature.firstResult())
  assert(aPointResult is not None)
  assert(aPointResult.isInfinite())


salome.salome_init()

###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 0, 0.5, 1.3)
model.do()

### Create Edge
Edge_1 = model.addEdge(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], False)

### Create LinearCopy
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("EDGE", "Edge_1_1")], model.selection("EDGE", "PartSet/OX"), 4, 5, keepSubResults = True)

### Create Plane
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), True)
Plane_4.setName("Plane_2")
Plane_4.result().setName("Plane_2")
Plane_4.result().setTransparency(0.6)

### Create Plane
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_2"), 1, False)
Plane_5.setName("Plane_3")
Plane_5.result().setName("Plane_3")
Plane_5.result().setTransparency(0.6)

### Create Plane
Plane_6 = model.addPlane(Part_1_doc, model.selection("EDGE", "LinearCopy_1_1_2"), model.selection("VERTEX", "LinearCopy_1_1_2/MV:Translated&Sketch_1/SketchLine_1_EndVertex"), True)
Plane_6.setName("Plane_4")
Plane_6.result().setName("Plane_4")
Plane_6.result().setTransparency(0.6)

### Create Plane
Plane_7 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_4"), 1, False)
Plane_7.setName("Plane_5")
Plane_7.result().setName("Plane_5")
Plane_7.result().setTransparency(0.6)

### Create Plane
Plane_8 = model.addPlane(Part_1_doc, model.selection("EDGE", "LinearCopy_1_1_3"), model.selection("VERTEX", "LinearCopy_1_1_3/MV:Translated&Sketch_1/SketchLine_1_EndVertex"), True)
Plane_8.setName("Plane_6")
Plane_8.result().setName("Plane_6")
Plane_8.result().setTransparency(0.6)

### Create Plane
Plane_9 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_6"), 1, False)
Plane_9.setName("Plane_7")
Plane_9.result().setName("Plane_7")
Plane_9.result().setTransparency(0.6)

### Create Point (by coordinates)
Point_1 = model.addPoint(Part_1_doc, 16, 0.5, 1.3)

### Create Point (by position on edge)
Point_2 = model.addPoint(Part_1_doc, model.selection("EDGE", "LinearCopy_1_1_4"), 2.5, False, False)

### Create Point (by projection of point on plane)
Point_3 = model.addPoint(Part_1_doc, model.selection("VERTEX", "LinearCopy_1_1_3/MV:Translated&Sketch_1/SketchLine_1_EndVertex"), model.selection("FACE", "Plane_7"))

### Create Point (by intersection of axis and plane)
Point_4 = model.addPoint(Part_1_doc, model.selection("EDGE", "LinearCopy_1_1_2"), model.selection("FACE", "Plane_5"))

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))

### Create SketchProjection
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchLine_1_EndVertex"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchCircle
SketchCircle_1 = Sketch_2.addCircle(0, 0, 0.6)
Sketch_2.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
Sketch_2.setRadius(SketchCircle_1.results()[1], 0.6)
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2f")], model.selection(), model.selection("FACE", "Plane_3"), 0, model.selection(), 0, "Faces|Wires")
Extrusion_1.result().setTransparency(0.6)

### Create Point (by center of gravity)
Point_5 = model.addPoint(Part_1_doc, model.selection("SOLID", "Extrusion_1_1"))

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "Point_1"), 0.8)

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "Point_2"), model.selection("EDGE", "LinearCopy_1_1_2"), 0.8, 1)

### Create Torus
Torus_1 = model.addTorus(Part_1_doc, model.selection("VERTEX", "Point_3"), model.selection("EDGE", "LinearCopy_1_1_3"), 0.8, 0.25)

### Create Cone
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "Point_4"), model.selection("EDGE", "LinearCopy_1_1_4"), 0.8, 0.3, 1)

### Create Sphere
Sphere_2 = model.addSphere(Part_1_doc, model.selection("VERTEX", "Point_5"), 0.8)

model.end()

assertPoint(Point_1)
assertPoint(Point_2)
assertPoint(Point_3)
assertPoint(Point_4)
assertPoint(Point_5)

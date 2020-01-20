# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10, 180)
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Cylinder_1_1/Face_5"))
SketchCircle_1 = Sketch_1.addCircle(-0.87355746875896, 7.873567272779828, 3.095312696967586)
model.do()
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2r")], model.selection(), [model.selection("SOLID", "Cylinder_1_1")])
Rotation_1 = model.addRotation(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_1_1")], model.selection("EDGE", "PartSet/OX"), 45)
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "[Rotation_1_1/MF:Rotated&Cylinder_1_1/Face_1][(Rotation_1_1/MF:Rotated&Cylinder_1_1/Face_1)(Rotation_1_1/MF:Rotated&Cylinder_1_1/Face_5)(Rotation_1_1/MF:Rotated&Cylinder_1_1/Face_4)(Rotation_1_1/MF:Rotated&Cylinder_1_1/Face_3)2]")], False)

Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Edge_1_1"), True)
SketchBSpline_1 = SketchProjection_1.createdFeature()
model.do()

Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchProjection_2 = Sketch_3.addProjection(model.selection("EDGE", "Edge_1_1"), True)
SketchBSpline_2 = SketchProjection_2.createdFeature()
model.do()

Sketch_4 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchProjection_3 = Sketch_4.addProjection(model.selection("EDGE", "Edge_1_1"), True)
SketchBSpline_3 = SketchProjection_3.createdFeature()
model.do()

model.end()

from GeomAPI import *
import math

TOLERANCE = 1.e-7

def checkProjection(theBSpline3D, theBSpline2D, theFlags):
    assert(theBSpline2D.isEdge() and theBSpline2D.edge().isBSpline())
    poles2D = GeomAPI_BSpline(GeomAPI_Curve(theBSpline2D)).poles()
    poles3D = theBSpline3D.poles()
    assert(poles2D.size() == poles3D.size())
    for p2d, p3d in zip(poles2D, poles3D):
        assert(math.fabs((p2d.x() - p3d.x()) * theFlags.x()) < TOLERANCE and
               math.fabs((p2d.y() - p3d.y()) * theFlags.y()) < TOLERANCE and
               math.fabs((p2d.z() - p3d.z()) * theFlags.z()) < TOLERANCE)


bspline0 = GeomAPI_BSpline(GeomAPI_Curve(Edge_1.results()[-1].resultSubShapePair()[0].shape()))

bsplineShape1 = SketchBSpline_1.results()[-1].resultSubShapePair()[0].shape()
checkProjection(bspline0, bsplineShape1, GeomAPI_Pnt(1, 1, 0))

bsplineShape2 = SketchBSpline_2.results()[-1].resultSubShapePair()[0].shape()
checkProjection(bspline0, bsplineShape2, GeomAPI_Pnt(1, 0, 1))

bsplineShape3 = SketchBSpline_3.results()[-1].resultSubShapePair()[0].shape()
checkProjection(bspline0, bsplineShape3, GeomAPI_Pnt(0, 1, 1))

assert(model.checkPythonDump())

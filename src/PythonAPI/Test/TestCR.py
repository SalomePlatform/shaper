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

from salome.shaper import model, geom

def isCRImplemented():
    CR = geom.CanonicalRecognition()
    return CR.isImplemented()

def GetShapeType(theShape):
    CR = geom.CanonicalRecognition()
    if CR.isLine(theShape, 0.1)[0]:
        return "Line"
    if CR.isPlane(theShape, 0.1)[0]:
        if CR.isCircle(theShape, 0.1)[0]:
            return ("Plane","Circle")
        if CR.isEllipse(theShape, 0.1)[0]:
            return ("Plane","Ellipse")
        return "Plane"
    if CR.isSphere(theShape, 0.1)[0]:
        return "Sphere"
    if CR.isCone(theShape, 0.1)[0]:
        return "Cone"
    if CR.isCylinder(theShape, 0.1)[0]:
        return "Cylinder"
    return "Not defined"


model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

### Create Shell
Plane_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Cylinder_1_1/Face_2")])

### Create Recover
Recover_1 = model.addRecover(Part_1_doc, Plane_1, [Cylinder_1.result()])

### Create Shell
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Recover_1_1/Modified_Face&Cylinder_1_1/Face_1")])

### Create Wire
Circle_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "[Recover_1_1/Modified_Face&Cylinder_1_1/Face_1][new_weak_name_2]")], False)

### Create Recover
Recover_2 = model.addRecover(Part_1_doc, Circle_1, [Shell_2.result()])

### Create Edge
Line_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "(Recover_2_1/Modified_Edge&Wire_1_1/Edge)")], False)

### Create Recover
Cylinder_3 = model.addRecover(Part_1_doc, Line_1, [Recover_2.result()])

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
SphereShell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Sphere_1_1/Face_1")])

### Create Cone
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 0, 10)
ConeShell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Cone_1_1/Face_1")])

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchEllipse
SketchEllipse_1 = Sketch_1.addEllipse(-37.39690313669599, -52.92905511857006, 61.08141071866921, -16.40734649833807, 41.5747486523393)
[SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchPoint_8, SketchLine_1, SketchLine_2] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
model.do()

### Create Wire
EllipseWire_1 = model.addWire(Part_1_doc, [model.selection("EDGE", "Sketch_1/SketchEllipse_1")], False)

model.end()

### Get shapes for tests
aPlaneShape = Plane_1.defaultResult().shape()
aCircleShape = Circle_1.defaultResult().shape()
aLineShape = Line_1.defaultResult().shape()
aCylinderShape = Cylinder_3.defaultResult().shape()
aSphereShape = SphereShell_1.defaultResult().shape()
aConeShape = ConeShell_1.defaultResult().shape()
aEllipseShape = EllipseWire_1.defaultResult().shape()

### Check shapes types
if isCRImplemented():
    assert (GetShapeType(aPlaneShape) == "Plane")
    assert (GetShapeType(aCircleShape)[1] == "Circle")
    assert (GetShapeType(aLineShape) == "Line")
    assert (GetShapeType(aCylinderShape) == "Cylinder")
    assert (GetShapeType(aSphereShape) == "Sphere")
    assert (GetShapeType(aConeShape) == "Cone")
    assert (GetShapeType(aEllipseShape)[1] == "Ellipse")

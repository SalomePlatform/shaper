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
from salome.shaper import geom
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
### Create Parallelepiped
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(100, 30, -100, 30)
SketchLine_2 = Sketch_1.addLine(-100, 30, -100, -12)
SketchLine_3 = Sketch_1.addLine(-100, -12, 100, -12)
SketchLine_4 = Sketch_1.addLine(100, -12, 100, 30)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1r-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 8, 0, "Faces|Wires")

### Create tool-set: all the needed types of edges and surfaces
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Extrusion_1_1/To_Face"))
SketchCircle_1 = Sketch_2.addCircle(-80, 10, 5)
SketchEllipse_1 = Sketch_2.addEllipse(-51, 10, -39, 10, 5)
SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
# moon
SketchArc_1 = Sketch_2.addArc(-22, 10, -22, 2, -22, 18, False)
SketchEllipticArc_1 = Sketch_2.addEllipticArc(-22, 10, -22, 17, -22, 2, -22, 18, False)
SketchEllipticArc_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
# cone
SketchLine_9 = Sketch_2.addLine(3, 14, 3, -2)
SketchLine_10 = Sketch_2.addLine(3, -2, 11, 14)
SketchLine_11 = Sketch_2.addLine(11, 14, 3, 14)
# tore
SketchLine_12 = Sketch_2.addLine(39, 16, 39, 1)
SketchLine_12.setAuxiliary(True)
SketchCircle_2 = Sketch_2.addCircle(48, 10, 5)
# circle
SketchLine_13 = Sketch_2.addLine(76, 14, 76, 0)
SketchArc_2 = Sketch_2.addArc(76, 7, 76, 14, 76, 0, True)
model.do()
### Create extrusions and revolutions
Extrusion_2_objects = [model.selection("FACE", "Sketch_2/Face-SketchCircle_1_2r"),
                       model.selection("FACE", "Sketch_2/Face-SketchEllipse_1f"),
                       model.selection("FACE", "Sketch_2/Face-SketchArc_1_2f-SketchEllipticArc_1r")]
Extrusion_2 = model.addExtrusion(Part_1_doc, Extrusion_2_objects, model.selection(), 8, 0, "Faces|Wires")
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_9f-SketchLine_10f-SketchLine_11f")], model.selection("EDGE", "Sketch_2/SketchLine_9"), 180, 0)
Revolution_2 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchCircle_2_2f")], model.selection("EDGE", "Sketch_2/SketchLine_12"), 180, 0)
Revolution_3 = model.addRevolution(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_13f-SketchArc_2_2f")], model.selection("EDGE", "Sketch_2/SketchLine_13"), 180, 0)
model.do()
### Fuse all
Fuse_1_objects_1 = [model.selection("SOLID", "Extrusion_1_1"),
                    model.selection("SOLID", "Extrusion_2_1"),
                    model.selection("SOLID", "Extrusion_2_2"),
                    model.selection("SOLID", "Extrusion_2_3"),
                    model.selection("SOLID", "Revolution_1_1"),
                    model.selection("SOLID", "Revolution_2_1"),
                    model.selection("SOLID", "Revolution_3_1")]
Fuse_1 = model.addFuse(Part_1_doc, Fuse_1_objects_1, keepSubResults = True)
model.end()

# a method to compare tupes: element by element, double values are compared with precision 1.e-9
def compareTuples(theT1, theT2):
  assert(len(theT1) == len(theT2))
  for i in range(len(theT1)):
    assert(type(theT1[i]) == type(theT2[i]))
    if type(theT1[i]) == float:
      #print("Compare " + "{:0.9f}".format(theT1[i]) + " and " + "{:0.9f}".format(theT2[i]));
      assert("{:0.9f}".format(theT1[i]) == "{:0.9f}".format(theT2[i]))
    else:
      assert(theT1[i] == theT2[i])


# check the whole solid
aSolid = Fuse_1.feature().firstResult().shape()
anInfo = geom.shapeInfo(aSolid)
compareTuples(anInfo, ('Solid',))
# check the shell
aShell = aSolid.subShapes(GeomAPI_Shape.SHELL)[0]
anInfo = geom.shapeInfo(aShell)
compareTuples(anInfo, ('Shell',))
# check faces
aFaces = aShell.subShapes(GeomAPI_Shape.FACE)
compareTuples(geom.shapeInfo(aFaces[0]), ('Rectangle', 100.0, 30.0, 8.0, -0.0, 1.0, -0.0, 8.0, 200.0))
compareTuples(geom.shapeInfo(aFaces[6]), ('Cone', 3.0, -2.0, 8.0, -0.0, 1.0, -0.0, 0.0, 8.0, 16.))
compareTuples(geom.shapeInfo(aFaces[8]), ('Sphere', 76.0, 7.0, 8.0, 7.0))
compareTuples(geom.shapeInfo(aFaces[9]), ('Cylinder', -80.0, 10.0, 16.0, -0.0, -0.0, -1.0, 5.0, 8.0))
compareTuples(geom.shapeInfo(aFaces[10]), ('Face',)) # linear extrusion surface
compareTuples(geom.shapeInfo(aFaces[13]), ('Torus', 39.0, 10.0, 8.0, -0.0, 1.0, -0.0, 9.0, 5.0))
compareTuples(geom.shapeInfo(aFaces[14]), ('Disk', -80.0, 10.0, 16.0, 0.0, 0.0, 1.0, 5.0)) # circular disk
compareTuples(geom.shapeInfo(aFaces[15]), ('Disk', -51.0, 10.0, 16.0, 0.0, 0.0, 1.0, 13.0, 5.0)) # elliptical disk
compareTuples(geom.shapeInfo(aFaces[16]), ('Plane', 0.0, 0.0, 16.0, 0.0, 0.0, 1.0))
# check wires
aWires = aShell.subShapes(GeomAPI_Shape.WIRE)
compareTuples(geom.shapeInfo(aWires[0]), ('Polygon', True, 100.0, 30.0, 8.0, 100.0, 30.0, 0.0, -100.0, 30.0, 0.0, -100.0, 30.0, 8.0))
compareTuples(geom.shapeInfo(aWires[6]), ('Wire', True))
# check edges
anEdges = aFaces[0].subShapes(GeomAPI_Shape.EDGE)
compareTuples(geom.shapeInfo(anEdges[0]), ('Line segment', 100.0, 30.0, 0.0, 100.0, 30.0, 8.0))
anEdges = aFaces[6].subShapes(GeomAPI_Shape.EDGE) # cone
compareTuples(geom.shapeInfo(anEdges[0]), ('Edge', True)) # Degenerated edge
compareTuples(geom.shapeInfo(anEdges[1]), ('Arc of circle', 3.0, 14.0, 8.0, 0.0, -1.0, 0.0, 8.0, 11.0, 14.0, 8.0, -5.0, 14.0, 8.0))
anEdges = aFaces[9].subShapes(GeomAPI_Shape.EDGE) # cylinder
compareTuples(geom.shapeInfo(anEdges[2]), ('Circle', -80.0, 10.0, 8.0, 0.0, 0.0, 1.0, 5.0))
anEdges = aFaces[10].subShapes(GeomAPI_Shape.EDGE) # ellipse
compareTuples(geom.shapeInfo(anEdges[2]), ('Ellipse', -51.0, 10.0, 8.0, 0.0, 0.0, 1.0, 13.0, 5.0))
anEdges = aFaces[16].subShapes(GeomAPI_Shape.EDGE) # moon top
compareTuples(geom.shapeInfo(anEdges[1]), ('Arc of ellipse', -22.0, 10.0, 16.0, 0.0, -0.0, 1.0, 8.0, 3.872983346207417, -22.0, 2.0, 16.0, -22.0, 18.0, 16.0))
# check vertex
aVertex = aSolid.subShapes(GeomAPI_Shape.VERTEX)[0]
compareTuples(geom.shapeInfo(aVertex), ('Vertex', 100.0, 30.0, 0.0))

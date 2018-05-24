## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
    TestIntersectionChangeEdge.py
    Unit test of SketchPlugin_IntersectionPoint class
"""

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchArc_1 = Sketch_1.addArc(0, 0, 13.75, -26.66341125962693, -24.98309575119448, 16.60857991180005, False)
SketchArc_2 = Sketch_1.addArc(0, 0, 53.75, -26.66341125962693, -53.75, -26.66341125962693, False)
SketchLine_1 = Sketch_1.addLine(13.75, -26.66341125962693, 53.75, -26.66341125962693)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_1.endPoint())
SketchLine_2 = Sketch_1.addLine(-24.98309575119448, 16.60857991180005, -53.75, -26.66341125962693)
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_2.endPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintCoincidence_5 = Sketch_1.setCoincident(SketchArc_2.center(), SketchArc_1.center())
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
SketchConstraintCoincidence_6 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_1.result())
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchArc_1.results()[1], 30)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintCoincidence_7 = Sketch_1.setCoincident(SketchArc_2.center(), SketchAPI_Point(SketchPoint_1).coordinates())
SketchConstraintRadius_2 = Sketch_1.setRadius(SketchArc_2.results()[1], 60)
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_1.result(), 40)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchArc_1_2r-SketchArc_2_2f-SketchLine_1f-SketchLine_2r")], model.selection(), 50, 50)
Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/From_Face_1"), model.selection("EDGE", "Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1")])
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/To_Face_1"), model.selection("VERTEX", "Extrusion_1_1/Generated_Face_2&Extrusion_1_1/Generated_Face_1&Extrusion_1_1/From_Face_1"), False)
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/Vertex-SketchArc_1-SketchArc_2-SketchProjection_1-SketchPoint_1"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()
SketchLine_3 = Sketch_2.addLine(-70, 60, 70, 60)
SketchLine_4 = Sketch_2.addLine(70, 60, 70, -60)
SketchLine_5 = Sketch_2.addLine(70, -60, -70, -60)
SketchLine_6 = Sketch_2.addLine(-70, -60, -70, 60)
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_2 = Sketch_2.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_2.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_5.result(), 60, True)
SketchConstraintDistance_2 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_3.result(), 60, True)
SketchConstraintDistance_3 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_4.result(), 70, True)
SketchConstraintDistance_4 = Sketch_2.setDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_6.result(), 70, True)
model.do()
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Sketch_2/Face-SketchLine_3r-SketchLine_4r-SketchLine_5r-SketchLine_6r")])
Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("FACE", "Filling_1_1")], [model.selection("FACE", "Face_1_1")])

# set different edges used for intersection and check reference data: number of intersection points and their coordinates
REF_DATA = [("Sketch_1/Edge-SketchArc_1_2", 1, [[30, 0]]),
            ("Sketch_1/Edge-SketchLine_2", 1, [[-36.024358588836, 0]]),
            ("Sketch_1/Edge-SketchArc_2_2", 2, [[60, 0], [-60, 0]]),
            ("Sketch_1/Edge-SketchLine_1", 0, []),
            ("Extrusion_1_1/Generated_Face_1&Extrusion_1_1/To_Face_1", 1, [[30, 50]]),
            ("Extrusion_1_1/Generated_Face_2&Extrusion_1_1/To_Face_1", 1, [[-36.024358588836, 50]]),
            ("Extrusion_1_1/Generated_Face_3&Extrusion_1_1/To_Face_1", 2, [[60, 50], [-60, 50]]),
            ("Extrusion_1_1/Generated_Face_4&Extrusion_1_1/To_Face_1", 0, []),
            ("Intersection_1_1", 2, [[-0.0515933488223, -11.6181750315], [41.6438615258, -11.6181750315]])]
TOLERANCE = 1.e-7

Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchIntersectionPoint_1 = Sketch_3.addIntersectionPoint(model.selection("EDGE", "Intersection_1_1"), True)
model.do()

for ref in REF_DATA:
    SketchIntersectionPoint_1.setByExternalEdgeName(ref[0])
    aPoints = SketchIntersectionPoint_1.intersectionPoints()
    assert(len(aPoints) == ref[1]), "Intersection with edge {} produce {} points, expected {}".format(ref[0], aPoints.size(), ref[1])
    # check coordinates
    ind = 0
    for p in aPoints:
        pnt = SketchAPI_Point(p)
        delta = (pnt.coordinates().x() - ref[2][ind][0])**2 + (pnt.coordinates().y() - ref[2][ind][1])**2
        assert(delta < TOLERANCE * TOLERANCE), "Wrong coordinates of intersection with edge {}: ({}, {}) != expected ({}, {})".format(ref[0], pnt.coordinates().x(), pnt.coordinates().y(), ref[2][ind][0], ref[2][ind][1])
        ind += 1
model.do()

model.end()

assert(model.checkPythonDump())

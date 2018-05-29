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

from salome.shaper import model

import math

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ParamR = model.addParameter(Part_1_doc, "R", "50")
ParamL = model.addParameter(Part_1_doc, "L", "40")
ParamH = model.addParameter(Part_1_doc, "H", "20")
Point_2 = model.addPoint(Part_1_doc, 0, 100, 100)
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OX"), model.selection("VERTEX", "Point_1"), False)
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(50, 50, 50)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], "R")
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchConstraintTangent_1 = Sketch_1.setTangent(SketchLine_1.result(), SketchCircle_1.results()[1])
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_2.createdFeature()
SketchConstraintTangent_2 = Sketch_1.setTangent(SketchLine_2.result(), SketchCircle_1.results()[1])
SketchLine_3 = Sketch_1.addLine(70, 40, 30, 40)
SketchLine_4 = Sketch_1.addLine(30, 40, 30, 60)
SketchLine_5 = Sketch_1.addLine(30, 60, 70, 60)
SketchLine_6 = Sketch_1.addLine(70, 60, 70, 40)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_4.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_6.result())
SketchConstraintLength_1 = Sketch_1.setLength(SketchLine_3.result(), "L")
SketchConstraintLength_2 = Sketch_1.setLength(SketchLine_4.result(), "H")
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchLine_4.startPoint(), SketchCircle_1.center(), "L/2")
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchLine_4.startPoint(), SketchCircle_1.center(), "H/2")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchCircle_1_2f-SketchLine_3f-SketchLine_4f-SketchLine_5f-SketchLine_6f")], model.selection(), model.selection("FACE", "Plane_1"), 0, model.selection(), 10)
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchArc_1 = Sketch_2.addArc(100, 48.41229182762603, 87.5, 0, 87.5, 96.82458365525073, False)
SketchArc_2 = Sketch_2.addArc(0, 48.41229182762603, 87.5, 0, 87.5, 96.82458365525073, False)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchArc_1.startPoint(), SketchArc_2.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchArc_1.endPoint())
SketchConstraintRadius_2 = Sketch_2.setRadius(SketchArc_1.results()[1], "R")
SketchConstraintRadius_3 = Sketch_2.setRadius(SketchArc_2.results()[1], "2*R")
SketchLine_7 = Sketch_2.addLine(0, 48.41229182762603, 100, 48.41229182762603)
SketchLine_7.setAuxiliary(True)
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchArc_2.center(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchArc_1.center(), SketchLine_7.endPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchProjection_3 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_8 = SketchProjection_3.createdFeature()
SketchConstraintCoincidence_9 = Sketch_2.setCoincident(SketchArc_1.startPoint(), SketchLine_8.result())
SketchProjection_4 = Sketch_2.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_9 = SketchProjection_4.createdFeature()
SketchConstraintCoincidence_10 = Sketch_2.setCoincident(SketchLine_7.startPoint(), SketchLine_9.result())
SketchConstraintCoincidence_11 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchArc_2.results()[1])
SketchConstraintMirror_1 = Sketch_2.addMirror(SketchLine_9.result(), [SketchArc_2.results()[1], SketchArc_1.results()[1]])
[SketchArc_3, SketchArc_4] = SketchConstraintMirror_1.mirrored()
model.do()
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection(), 100, 0)
model.do()

TOLERANCE = 1.e-6

# reference data
REF_DATA = [(model.selection("VERTEX", "PartSet/Origin"),
             model.selection("VERTEX", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/To_Face_1"),
             122.4744871),
            (model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/To_Face_1"),
             model.selection("VERTEX", "Sketch_2/Vertex-SketchArc_1-SketchLine_7e"),
             36.94403089),
            (model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/From_Face_1"),
             model.selection("EDGE", "Extrusion_1_1/Generated_Face_5"),
             0),
            (model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/From_Face_1"),
             model.selection("EDGE", "Extrusion_2_1/Generated_Face_2&Extrusion_2_1/To_Face_1"),
             16.00781059),
            (model.selection("EDGE", "Extrusion_1_1/Generated_Face_5&Extrusion_1_1/From_Face_1"),
             model.selection("FACE", "Extrusion_2_1/Generated_Face_2"),
             8.412291828),
            (model.selection("VERTEX", "Sketch_1/Vertex-SketchCircle_1"),
             model.selection("FACE", "Plane_1"),
             35.35533906),
            (model.selection("FACE", "Extrusion_2_2/From_Face_1"),
             model.selection("FACE", "Extrusion_2_2/To_Face_1"),
             100),
            (model.selection("FACE", "Extrusion_1_1/Generated_Face_5"),
             model.selection("FACE", "Extrusion_2_1/Generated_Face_2"),
             0),
            (model.selection("FACE", "Extrusion_1_1/Generated_Face_5"),
             model.selection("FACE", "Extrusion_1_1/Generated_Face_2"),
             27.63932023),
            (model.selection("SOLID", "Extrusion_1_1"),
             model.selection("FACE", "Extrusion_2_1/To_Face_1"),
             12.5),
            (model.selection("SOLID", "Extrusion_1_1"),
             model.selection("SOLID", "Extrusion_2_1"),
             0),
            (model.selection("SOLID", "Extrusion_1_1"),
             model.selection("SOLID", "Extrusion_2_2"),
             87.5)
           ]

for ref in REF_DATA:
    dist = model.measureDistance(Part_1_doc, ref[0], ref[1])
    assert(math.fabs(dist - ref[2]) < TOLERANCE), "Distance {} differs from expected value {}".format(dist, ref[2])

model.end()

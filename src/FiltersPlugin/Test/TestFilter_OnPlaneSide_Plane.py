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

from SketchAPI import *
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1")], keepSubResults = True)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], model.selection("EDGE", "PartSet/OX"), 20)
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(-15, 4, 10)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_1.results()[1], 10)
SketchLine_1 = Sketch_1.addLine(-24.53939201417305, 7, -5.460607985826952, 7)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchCircle_1.results()[1])
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchCircle_1.results()[1])
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintDistance_1 = Sketch_1.setDistance(SketchCircle_1.center(), SketchLine_1.result(), 3, True)
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Bottom][Box_1_1/Left]"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
SketchConstraintDistanceHorizontal_1 = Sketch_1.setHorizontalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_1).coordinates(), 15)
SketchConstraintDistanceVertical_1 = Sketch_1.setVerticalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), 4)
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), 15, 5)
Filters = model.filters(Part_1_doc, [model.addFilter(name = "OnPlaneSide", args = [model.selection("FACE", "Plane_1")])])
model.end()

ResultBox_1 = Partition_1.result().subResult(0).resultSubShapePair()[0]
ResultBox_2 = Partition_1.result().subResult(1).resultSubShapePair()[0]
ResultCylinder = Cylinder_1.result().resultSubShapePair()[0]
ResultExtrusion_1 = Extrusion_1.result().subResult(0).resultSubShapePair()[0]
ResultExtrusion_2 = Extrusion_1.result().subResult(1).resultSubShapePair()[0]

from GeomAPI import GeomAPI_Shape
emptyShape = GeomAPI_Shape()

Reference = {
    # Solids
    model.selection(ResultBox_1, emptyShape): True,
    model.selection(ResultBox_2, emptyShape): False,
    model.selection(ResultCylinder, emptyShape): True,
    model.selection(ResultExtrusion_1, emptyShape): True,
    model.selection(ResultExtrusion_2, emptyShape): False,
    # Faces of the box
    model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Back"): True,
    model.selection("FACE", "Box_1_1/Left"): True,
    model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Bottom"): True,
    model.selection("FACE", "Partition_1_1_1/Modified_Face&Plane_1/Plane_1"): False,
    model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Top"): True,
    model.selection("FACE", "Partition_1_1_1/Modified_Face&Box_1_1/Front"): True,
    model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Back"): False,
    model.selection("FACE", "Partition_1_1_2/Modified_Face&Plane_1/Plane_1"): False,
    model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Bottom"): False,
    model.selection("FACE", "Box_1_1/Right"): False,
    model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Top"): False,
    model.selection("FACE", "Partition_1_1_2/Modified_Face&Box_1_1/Front"): False,
    # Faces of the extrusion
    model.selection("FACE", "Extrusion_1_1_1/From_Face"): True,
    model.selection("FACE", "Extrusion_1_1_1/To_Face"): True,
    model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1"): True,
    model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2"): False,
    model.selection("FACE", "Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1"): False,
    model.selection("FACE", "Extrusion_1_1_2/From_Face"): False,
    model.selection("FACE", "Extrusion_1_1_2/To_Face"): False,
    model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1"): False,
    model.selection("FACE", "Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2"): False,
    # Faces of the cylinder
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_1"): True,
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_2"): True,
    model.selection("FACE", "Translation_1_1/MF:Translated&Cylinder_1_1/Face_3"): True,
    # Edges of the box
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Bottom]"): True,
    model.selection("EDGE", "Partition_1_1_1/Generated_Edge&Plane_1/Plane_1&Box_1_1/Back"): False,
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Back][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Top]"): True,
    model.selection("EDGE", "Partition_1_1_1/Generated_Edge&Plane_1/Plane_1&Box_1_1/Bottom"): False,
    model.selection("EDGE", "Partition_1_1_1/Generated_Edge&Plane_1/Plane_1&Box_1_1/Top"): False,
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Top][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Bottom][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Bottom][Partition_1_1_1/Modified_Face&Box_1_1/Front]"): True,
    model.selection("EDGE", "Partition_1_1_1/Generated_Edge&Plane_1/Plane_1&Box_1_1/Front"): False,
    model.selection("EDGE", "[Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Front]"): True,
    model.selection("EDGE", "[Partition_1_1_1/Modified_Face&Box_1_1/Top][Partition_1_1_1/Modified_Face&Box_1_1/Front]"): True,
    model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Back][Partition_1_1_2/Modified_Face&Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Back][Box_1_1/Right]"): False,
    model.selection("EDGE", "Partition_1_1_2/Generated_Edge&Plane_1/Plane_1&Box_1_1/Back"): False,
    model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Back][Partition_1_1_2/Modified_Face&Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Partition_1_1_2/Modified_Face&Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Top][Box_1_1/Right]"): False,
    model.selection("EDGE", "Partition_1_1_2/Generated_Edge&Plane_1/Plane_1&Box_1_1/Top"): False,
    model.selection("EDGE", "Partition_1_1_2/Generated_Edge&Plane_1/Plane_1&Box_1_1/Bottom"): False,
    model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Front]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Partition_1_1_2/Modified_Face&Box_1_1/Front]"): False,
    model.selection("EDGE", "Partition_1_1_2/Generated_Edge&Plane_1/Plane_1&Box_1_1/Front"): False,
    model.selection("EDGE", "[Partition_1_1_2/Modified_Face&Box_1_1/Top][Partition_1_1_2/Modified_Face&Box_1_1/Front]"): False,
    # Edges of the extrusion
    model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_2/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_2/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1][weak_name_2]"): False,
    model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1][weak_name_1]"): False,
    model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchCircle_1_2][Extrusion_1_1_2/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_2/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_2/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/From_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/From_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1]"): False,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2]"): True,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1]"): False,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_1/To_Face]"): False,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/To_Face]"): True,
    model.selection("EDGE", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/To_Face]"): False,
    # Edges of the cylinder
    model.selection("EDGE", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2]"): True,
    model.selection("EDGE", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_3]"): True,
    model.selection("EDGE", "([Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2])([Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_3])"): True,
    # Vertices of the box
    model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_1_1/Top][Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Front]"): True,
    model.selection("VERTEX", "Partition_1_1_1/Generated_Vertex&Plane_1/Plane_1&weak_name_4"): False,
    model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Top][Box_1_1/Left]"): True,
    model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_1_1/Bottom][Box_1_1/Left][Partition_1_1_1/Modified_Face&Box_1_1/Front]"): True,
    model.selection("VERTEX", "Partition_1_1_1/Generated_Vertex&Plane_1/Plane_1&weak_name_2"): False,
    model.selection("VERTEX", "Partition_1_1_1/Generated_Vertex&Plane_1/Plane_1&weak_name_3"): False,
    model.selection("VERTEX", "[Partition_1_1_1/Modified_Face&Box_1_1/Back][Partition_1_1_1/Modified_Face&Box_1_1/Bottom][Box_1_1/Left]"): True,
    model.selection("VERTEX", "Partition_1_1_1/Generated_Vertex&Plane_1/Plane_1&weak_name_1"): False,
    model.selection("VERTEX", "Partition_1_1_2/Generated_Vertex&Plane_1/Plane_1&weak_name_3"): False,
    model.selection("VERTEX", "[Partition_1_1_2/Modified_Face&Box_1_1/Back][Partition_1_1_2/Modified_Face&Box_1_1/Top][Box_1_1/Right]"): False,
    model.selection("VERTEX", "[Box_1_1/Right][Partition_1_1_2/Modified_Face&Box_1_1/Bottom][Partition_1_1_2/Modified_Face&Box_1_1/Front]"): False,
    model.selection("VERTEX", "Partition_1_1_2/Generated_Vertex&Plane_1/Plane_1&weak_name_1"): False,
    model.selection("VERTEX", "[Partition_1_1_2/Modified_Face&Box_1_1/Back][Box_1_1/Right][Partition_1_1_2/Modified_Face&Box_1_1/Bottom]"): False,
    model.selection("VERTEX", "Partition_1_1_2/Generated_Vertex&Plane_1/Plane_1&weak_name_2"): False,
    model.selection("VERTEX", "Partition_1_1_2/Generated_Vertex&Plane_1/Plane_1&weak_name_4"): False,
    model.selection("VERTEX", "[Partition_1_1_2/Modified_Face&Box_1_1/Top][Box_1_1/Right][Partition_1_1_2/Modified_Face&Box_1_1/Front]"): False,
    # Vertices of the extrusion
    model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/To_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_1/To_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/From_Face]"): True,
    model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_2][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_1/From_Face]"): False,
    model.selection("VERTEX", "[Extrusion_1_1_1/Generated_Face&Sketch_1/SketchCircle_1_2&weak_name_1][Extrusion_1_1_1/Generated_Face&Sketch_1/SketchLine_1][Extrusion_1_1_1/From_Face]"): False,
    model.selection("VERTEX", "[_weak_name_6_Extrusion_1_1_2]e[_weak_name_5_Extrusion_1_1_2]e[_weak_name_3_Extrusion_1_1_2]e"): False,
    model.selection("VERTEX", "[_weak_name_1_Extrusion_1_1_2]e[_weak_name_5_Extrusion_1_1_2]e[_weak_name_3_Extrusion_1_1_2]e"): False,
    model.selection("VERTEX", "[_weak_name_6_Extrusion_1_1_2]e[_weak_name_4_Extrusion_1_1_2]e[_weak_name_2_Extrusion_1_1_2]e"): False,
    model.selection("VERTEX", "[_weak_name_1_Extrusion_1_1_2]e[_weak_name_4_Extrusion_1_1_2]e[_weak_name_2_Extrusion_1_1_2]e"): False,
    # Vertices of the cylinder
    model.selection("VERTEX", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_2]"): True,
    model.selection("VERTEX", "[Translation_1_1/MF:Translated&Cylinder_1_1/Face_1][Translation_1_1/MF:Translated&Cylinder_1_1/Face_3]"): True,
}
model.checkFilter(Part_1_doc, model, Filters, Reference)

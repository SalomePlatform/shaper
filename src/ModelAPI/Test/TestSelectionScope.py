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

from GeomAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
lenParam = model.addParameter(Part_1_doc, "len", "50")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchLine_1 = Sketch_1.addLine(33.62835038363171, 25.06143222506393, -25.8286189258312, 25.06143222506393)
SketchLine_2 = Sketch_1.addLine(-25.8286189258312, 25.06143222506393, -25.8286189258312, -21.22549872122762)
SketchLine_3 = Sketch_1.addLine(-25.8286189258312, -21.22549872122762, 33.62835038363171, -21.22549872122762)
SketchLine_4 = Sketch_1.addLine(33.62835038363171, -21.22549872122762, 33.62835038363171, 25.06143222506393)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_4 = Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
SketchConstraintHorizontal_1 = Sketch_1.setHorizontal(SketchLine_1.result())
SketchConstraintVertical_1 = Sketch_1.setVertical(SketchLine_2.result())
SketchConstraintHorizontal_2 = Sketch_1.setHorizontal(SketchLine_3.result())
SketchConstraintVertical_2 = Sketch_1.setVertical(SketchLine_4.result())
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_1f-SketchLine_2f-SketchLine_3f-SketchLine_4f")], model.selection(), 10, 0)
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [], model.selection(), 5, 5, [model.selection("SOLID", "Extrusion_1_1")])
Sketch_2_origin = GeomAPI_Pnt(0, -10, 0)
Sketch_2_norm = GeomAPI_Dir(0, -1, 0)
Sketch_2_dirx = GeomAPI_Dir(1, 0, 0)
Sketch_2 = model.addSketch(Part_1_doc, GeomAPI_Ax3(Sketch_2_origin, Sketch_2_dirx, Sketch_2_norm))
SketchLine_5 = Sketch_2.addLine(8.950511508951408, 45.06143222506392, -10.86847826086957, 45.06143222506392)
SketchLine_6 = Sketch_2.addLine(-10.86847826086957, 45.06143222506392, -10.86847826086957, -4.938567774936073)
SketchLine_7 = Sketch_2.addLine(-10.86847826086957, -4.938567774936073, 8.950511508951408, -4.938567774936073)
SketchLine_8 = Sketch_2.addLine(8.950511508951408, -4.938567774936073, 8.950511508951408, 45.06143222506392)
SketchConstraintCoincidence_5 = Sketch_2.setCoincident(SketchLine_8.endPoint(), SketchLine_5.startPoint())
SketchConstraintCoincidence_6 = Sketch_2.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
SketchConstraintCoincidence_7 = Sketch_2.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
SketchConstraintCoincidence_8 = Sketch_2.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
SketchConstraintHorizontal_3 = Sketch_2.setHorizontal(SketchLine_5.result())
SketchConstraintVertical_3 = Sketch_2.setVertical(SketchLine_6.result())
SketchConstraintHorizontal_4 = Sketch_2.setHorizontal(SketchLine_7.result())
SketchConstraintVertical_4 = Sketch_2.setVertical(SketchLine_8.result())
SketchConstraintLength_1 = Sketch_2.setLength(SketchLine_8.result(), "len")
SketchProjection_1 = Sketch_2.addProjection(model.selection("EDGE", "Extrusion_1_1/Generated_Face_4&Extrusion_1_1/To_Face_1"), False)
SketchLine_9 = SketchProjection_1.createdFeature()
SketchConstraintDistance_1 = Sketch_2.setDistance(SketchLine_5.startPoint(), SketchLine_9.result(), 20, True)
ExtrusionCut_1.setNestedSketch(Sketch_2)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "ExtrusionCut_1_1/Modfied_2&ExtrusionCut_1_1/Modfied_1&Extrusion_1_1/Generated_Face_1"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "ExtrusionCut_1_1/Modfied_2&Extrusion_1_1/Generated_Face_3&ExtrusionCut_1_1/Modfied_1"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
model.do()
# make sketch_2 cut the whole top face of the initial extrusion, so, one of the cylinder becomes invalid
lenParam.setValue(100)
Part_1_doc.setCurrentFeature(Cylinder_2.feature(), True)
model.do()
# update selection of cylinder base points
model.selection("VERTEX", "ExtrusionCut_1_1/Modfied_3&Extrusion_1_1/Generated_Face_3&ExtrusionCut_1_1/Modfied_1").fillAttribute(Cylinder_1.basePoint())
model.selection("VERTEX", "ExtrusionCut_1_1/Modfied_3&ExtrusionCut_1_1/Modfied_divided_1_1&Extrusion_1_1/Generated_Face_1").fillAttribute(Cylinder_2.basePoint())
model.end()

# check that both cylinders are valid and have correct results
assert(Cylinder_1.feature().results().size() == 1)
assert(Cylinder_2.feature().results().size() == 1)

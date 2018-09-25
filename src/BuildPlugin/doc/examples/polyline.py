# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Point_2 = model.addPoint(Part_1_doc, -10, -5, -7)
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Point_1")])
Polyline_1_objects = [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Left&Box_1_1/Bottom"), model.selection("VERTEX", "Box_1_1/Front&Box_1_1/Right&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Back&Box_1_1/Left&Box_1_1/Top"), model.selection("VERTEX", "Box_1_1/Back&Box_1_1/Right&Box_1_1/Bottom")]
Polyline_1 = model.addPolyline3D(Part_1_doc, Polyline_1_objects, False)
model.do()
model.end()

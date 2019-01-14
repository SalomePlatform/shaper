# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.do()
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Plane_4 = model.addPlane(Part_2_doc, model.selection("FACE", "PartSet/YOZ"), 100, False)
Plane_5 = model.addPlane(Part_2_doc, model.selection("FACE", "Plane_1"), 120, False)
Sketch_1 = model.addSketch(Part_2_doc, model.standardPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(-17.96171526586622, -340.274716981132, 71.31826129521251)
model.do()
Sketch_2 = model.addSketch(Part_2_doc, model.selection("FACE", "Plane_1"))
SketchCircle_2 = Sketch_2.addCircle(46.42844892447576, -229.9982953588847, 114.4484521366094)
model.do()
Sketch_3 = model.addSketch(Part_2_doc, model.selection("FACE", "Plane_2"))
SketchCircle_3 = Sketch_3.addCircle(121.2811188302094, -19.85285601110755, 120.5601403551194)
model.do()
Interpolation_1_objects = [model.selection("VERTEX", "Sketch_1/SketchCircle_1"), model.selection("VERTEX", "Sketch_2/SketchCircle_2"), model.selection("VERTEX", "Sketch_3/SketchCircle_3")]
Interpolation_1 = model.addInterpolation(Part_2_doc, Interpolation_1_objects, False, False)
Vertex_1 = model.addVertex(Part_2_doc, [model.selection("VERTEX", "Sketch_2/SketchCircle_2")])
Partition_1 = model.addPartition(Part_2_doc, [model.selection("EDGE", "Interpolation_1_1"), model.selection("VERTEX", "Vertex_1_1")])
Wire_1 = model.addWire(Part_2_doc, [model.selection("EDGE", "Partition_1_1_1"), model.selection("EDGE", "Partition_1_1_2")])
Pipe_1_objects_2 = [model.selection("VERTEX", "Interpolation_1_1/Vertex_1_2"), model.selection("VERTEX", "Interpolation_1_1/Vertex_1_1"), model.selection("VERTEX", "Vertex_1_1/Vertex_1_1&Sketch_2/SketchCircle_2")]
Pipe_1_objects_1 = [model.selection("WIRE", "Sketch_1/Face-SketchCircle_1_2r_wire"), model.selection("WIRE", "Sketch_2/Face-SketchCircle_2_2r_wire"), model.selection("FACE", "Sketch_3/Face-SketchCircle_3_2r")]
Pipe_1 = model.addPipe(Part_2_doc, Pipe_1_objects_1, model.selection("WIRE", "Wire_1_1"), Pipe_1_objects_2)
model.do()
model.end()

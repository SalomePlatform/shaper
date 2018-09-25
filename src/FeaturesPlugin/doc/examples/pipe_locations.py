# -*- coding: utf-8 -*-

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "PartSet/YOZ"), 100, False)
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Plane_1"), 120, False)
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("YOZ"))
SketchCircle_1 = Sketch_1.addCircle(-17.96171526586622, -340.274716981132, 71.31826129521251)
SketchCircle_1.result().setColor(225, 0, 0)
SketchCircle_1.results()[1].setColor(225, 0, 0)
model.do()
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_1"))
SketchCircle_2 = Sketch_2.addCircle(46.42844892447576, -229.9982953588847, 114.4484521366094)
SketchCircle_2.result().setColor(225, 0, 0)
SketchCircle_2.results()[1].setColor(225, 0, 0)
model.do()
Sketch_3 = model.addSketch(Part_1_doc, model.selection("FACE", "Plane_2"))
SketchCircle_3 = Sketch_3.addCircle(121.2811188302094, -19.85285601110755, 120.5601403551194)
SketchCircle_3.result().setColor(225, 0, 0)
SketchCircle_3.results()[1].setColor(225, 0, 0)
model.do()
Interpolation_1_objects = [model.selection("VERTEX", "Sketch_1/Vertex-SketchCircle_1"), model.selection("VERTEX", "Sketch_2/Vertex-SketchCircle_2"), model.selection("VERTEX", "Sketch_3/Vertex-SketchCircle_3")]
Interpolation_1 = model.addInterpolation(Part_1_doc, Interpolation_1_objects, False, False)
Edge_1_objects = [model.selection("EDGE", "Sketch_1/Edge-SketchCircle_1_2"), model.selection("EDGE", "Sketch_2/Edge-SketchCircle_2_2"), model.selection("EDGE", "Sketch_3/Edge-SketchCircle_3_2")]
Edge_1 = model.addEdge(Part_1_doc, Edge_1_objects)
Pipe_1_objects_1 = [model.selection("EDGE", "Edge_1_1"), model.selection("EDGE", "Edge_1_2"), model.selection("EDGE", "Edge_1_3")]
Pipe_1 = model.addPipe(Part_1_doc, Pipe_1_objects_1, model.selection("EDGE", "Interpolation_1_1"), [])
model.do()
model.end()

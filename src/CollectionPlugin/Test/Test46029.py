#!/usr/bin/env python

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Point
coords_A = (3, 4, 6)
x,y,z = coords_A
Point_2 = model.addPoint(Part_1_doc, x,y,z)
Point_2.result().setName("Point A")

### Create Point
coords_B = (8, 10, 20)
x,y,z = coords_B
Point_3 = model.addPoint(Part_1_doc, x,y,z)
Point_3.result().setName("Point B")

### Create Point
coords_C = (20, 10, 12)
x,y,z = coords_C
Point_4 = model.addPoint(Part_1_doc,x,y,z)
Point_4.result().setName("Point C")

### Create Edge
Edge_1 = model.addEdge(Part_1_doc,
			model.selection(Point_2.defaultResult()), 
			model.selection(Point_3.defaultResult()))
Edge_1.result().setName("Arete 1")

### Create Edge
Edge_2 = model.addEdge(Part_1_doc, 
		model.selection(Point_3.defaultResult()),
		model.selection(Point_4.defaultResult()))
Edge_2.result().setName("Arete 2")

### Create Groups
A = model.selection("VERTEX", coords_A)
Group_1 = model.addGroup(Part_1_doc, "Vertices", [A])
Group_1.setName("Groupe_point_A")
Group_1.result().setName("Groupe_point_A")

Group_2_objects = [A,
                   model.selection("VERTEX", coords_B),
                   model.selection("VERTEX", coords_C)]
Group_2 = model.addGroup(Part_1_doc, "Vertices", Group_2_objects)
Group_2.setName("Groupe_points_ABC")
Group_2.result().setName("Groupe_points_ABC")

assert(Group_2.feature().error() == "")

model.end()
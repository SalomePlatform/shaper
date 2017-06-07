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

"""
Test case for Translation feature.
Written on High API.
"""
from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Boxes
Box_1 = model.addBox(Part_1_doc, 10, 20, 10)

####################
# Sketches
# Sketch contenant un triangle
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_1 = Sketch_1.addLine(105.8795836373178, 106.9780351893485, -55.30451701345322, -118.6797057217308)
SketchLine_2 = Sketch_1.addLine(-55.30451701345322, -118.6797057217308, -134.9484255703047, 86.11891628160174)
SketchLine_3 = Sketch_1.addLine(-134.9484255703047, 86.11891628160174, 105.8795836373178, 106.9780351893485)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
SketchConstraintCoincidence_2 = Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
SketchConstraintCoincidence_3 = Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_3.endPoint())

# Sketch contenant un cercle et une ligne droite ne coupant pas le cercle, tous deux contenus dans l'une des faces de la box 1
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
SketchCircle_1 = Sketch_2.addCircle(3.625538020086079, 17.57352941176471, 2.190441696184978)
SketchLine_4 = Sketch_2.addLine(2.442711482557598, 6.454964041059345, 8.035432938365913, 18.18483116518828)

# Sketch contenant une ligne droite
Sketch_3 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_5 = Sketch_3.addLine(-42.70592498087446, -19.7671890801643, 76.68536514422158, 72.47712556405142)

# Sketch contenant 2 lignes droites independantes
Sketch_4 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_6 = Sketch_4.addLine(64.56241032998567, 126.9727403156384, -187.948350071736, -65.2797704447633)
SketchLine_7 = Sketch_4.addLine(147.7761836441895, 78.19225251076045, 2.869440459110503, -194.4045911047346)

# Sketch contenant un triangle et une ligne droite sans contact avec le triangle
Sketch_5 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_8 = Sketch_5.addLine(43.04160688665702, 82.49641319942614, -63.12769010043041, -3.586800573888105)
SketchLine_9 = Sketch_5.addLine(-63.12769010043041, -3.586800573888105, 60.25824964132004, -45.19368723098994)
SketchLine_10 = Sketch_5.addLine(60.25824964132004, -45.19368723098994, 43.04160688665702, 82.49641319942614)
SketchConstraintCoincidence_4 = Sketch_5.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())
SketchConstraintCoincidence_5 = Sketch_5.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
SketchConstraintCoincidence_6 = Sketch_5.setCoincident(SketchLine_8.startPoint(), SketchLine_10.endPoint())
SketchLine_11 = Sketch_5.addLine(-129.0707731585723, -64.87131938806517, -62.80680259595319, 54.77196079444149)

# Sketch contenant deux triangles ayant un cote en commun
Sketch_6 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_12 = Sketch_6.addLine(31.56384505021527, 81.06169296987088, -80.3443328550933, -15.06456241033)
SketchLine_13 = Sketch_6.addLine(-80.3443328550933, -15.06456241033, 58.82352941176466, -35.15064562410331)
SketchLine_14 = Sketch_6.addLine(58.82352941176466, -35.15064562410331, 31.56384505021527, 81.06169296987088)
SketchLine_15 = Sketch_6.addLine(31.56384505021527, 81.06169296987088, 159.2539454806312, 40.88952654232425)
SketchLine_16 = Sketch_6.addLine(159.2539454806312, 40.88952654232425, 58.82352941176466, -35.15064562410331)
SketchConstraintCoincidence_7 = Sketch_6.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())
SketchConstraintCoincidence_8 = Sketch_6.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())
SketchConstraintCoincidence_9 = Sketch_6.setCoincident(SketchLine_12.startPoint(), SketchLine_14.endPoint())
SketchConstraintCoincidence_10 = Sketch_6.setCoincident(SketchLine_12.startPoint(), SketchLine_15.startPoint())
SketchConstraintCoincidence_11 = Sketch_6.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())
SketchConstraintCoincidence_12 = Sketch_6.setCoincident(SketchLine_13.endPoint(), SketchLine_16.endPoint())

# Sketch contenant deux triangles ayant un cote en commun et une ligne droite ne coupant aucun des 2 triangles
Sketch_7 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchLine_17 = Sketch_7.addLine(17.21664275466276, 98.27833572453372, -144.906743185079, -46.6284074605452)
SketchLine_18 = Sketch_7.addLine(-144.906743185079, -46.6284074605452, 143.4720229555236, -105.4519368723099)
SketchLine_19 = Sketch_7.addLine(143.4720229555236, -105.4519368723099, 17.21664275466276, 98.27833572453372)
SketchLine_20 = Sketch_7.addLine(17.21664275466276, 98.27833572453372, 272.5968436154949, 99.71305595408899)
SketchLine_21 = Sketch_7.addLine(272.5968436154949, 99.71305595408899, 143.4720229555236, -105.4519368723099)
SketchConstraintCoincidence_13 = Sketch_7.setCoincident(SketchLine_17.endPoint(), SketchLine_18.startPoint())
SketchConstraintCoincidence_14 = Sketch_7.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
SketchConstraintCoincidence_15 = Sketch_7.setCoincident(SketchLine_17.startPoint(), SketchLine_19.endPoint())
SketchConstraintCoincidence_16 = Sketch_7.setCoincident(SketchLine_17.startPoint(), SketchLine_20.startPoint())
SketchConstraintCoincidence_17 = Sketch_7.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())
SketchConstraintCoincidence_18 = Sketch_7.setCoincident(SketchLine_18.endPoint(), SketchLine_21.endPoint())
SketchLine_22 = Sketch_7.addLine(-190.8177905308465, 62.41032998565277, -307.0301291248207, -188.6657101865136)

# Sketch contenant un cercle
Sketch_8 = model.addSketch(Part_1_doc, model.selection("FACE", "Box_1_1/Front"))
SketchCircle_2 = Sketch_8.addCircle(5.043472022955523, 6.180989956958394, 2.65480068754944)

model.do()

# Extrusions
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_2/Wire-SketchCircle_1_2f")], model.selection(), 15, 0)
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("WIRE", "Sketch_8/Wire-SketchCircle_2_2f")], model.selection(), 15, 0)

# Edges
Edge_1 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_1/Edge-SketchLine_2")])
Edge_2 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_3/Edge-SketchLine_5")])
Edge_3 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_3/Edge-SketchLine_5")])
Edge_4 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_3/Edge-SketchLine_5")])
Edge_5 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_3/Edge-SketchLine_5")])
Edge_6 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_3/Edge-SketchLine_5")])
Edge_7 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_4/Edge-SketchLine_7")])
Edge_8 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_4/Edge-SketchLine_6")])
Edge_9 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_4/Edge-SketchLine_7")])
Edge_10 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_4/Edge-SketchLine_7")])
Edge_11 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_5/Edge-SketchLine_11")])
Edge_12 = model.addEdge(Part_1_doc, [model.selection("EDGE", "Sketch_7/Edge-SketchLine_22")])

# Axis
Axis_1 = model.addAxis(Part_1_doc, 10, 10, 10)

# Vertex
Vertex_1 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_2 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_3 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_4 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_5 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_6 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_7 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_8 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])
Vertex_9 = model.addVertex(Part_1_doc, [model.selection("VERTEX", "Sketch_3/Vertex-SketchLine_5e")])

# Wires
Wire_1 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Wire_2 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Wire_3 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Wire_4 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Wire_5 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Wire_6 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_5/Wire-SketchLine_8f-SketchLine_9f-SketchLine_10f")])
Wire_7 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_5/Wire-SketchLine_8f-SketchLine_9f-SketchLine_10f")])
Wire_8 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Wire_9 = model.addWire(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])

# Faces
Face_1 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_2 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_3 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_4 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_5 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_6 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_5/Wire-SketchLine_8f-SketchLine_9f-SketchLine_10f")])
Face_7 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_5/Wire-SketchLine_8f-SketchLine_9f-SketchLine_10f")])
Face_8 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_9 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_1/Wire-SketchLine_1f-SketchLine_2f-SketchLine_3f")])
Face_10 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_6/Wire-SketchLine_12f-SketchLine_13f-SketchLine_14f")])
Face_11 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_6/Wire-SketchLine_14r-SketchLine_15r-SketchLine_16r")])
Face_12 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_7/Wire-SketchLine_17f-SketchLine_18f-SketchLine_19f")])
Face_13 = model.addFace(Part_1_doc, [model.selection("WIRE", "Sketch_7/Wire-SketchLine_19r-SketchLine_20r-SketchLine_21r")])

# Shells
Shell_1 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])
Shell_2 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])
Shell_3 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])
Shell_4 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])
Shell_5 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])
Shell_6 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_12_1"), model.selection("FACE", "Face_13_1")])
Shell_7 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_12_1"), model.selection("FACE", "Face_13_1")])
Shell_8 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])
Shell_9 = model.addShell(Part_1_doc, [model.selection("FACE", "Face_10_1"), model.selection("FACE", "Face_11_1")])

# Parameters
model.addParameter(Part_1_doc, "d", "15")

# Translations
Translation_45 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_1_1")], model.selection("EDGE", "PartSet/OX"), 15)
Translation_46 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_2_1")], model.selection("EDGE", "PartSet/OX"), 0)
Translation_47 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_3_1")], model.selection("EDGE", "PartSet/OX"), -15)
Translation_48 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_4_1")], model.selection("EDGE", "InvalidName"), 15)
Translation_49 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_5_1")], model.selection("EDGE", "PartSet/OX"), "d")
Translation_50 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_6_1")], model.selection("EDGE", "Sketch_3/Edge-SketchLine_5"), 15)
Translation_51 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_7_1")], model.selection("EDGE", "Edge_2_1"), 15)
Translation_52 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_8_1")], model.selection("EDGE", "Box_1_1/Back&Box_1_1/Right"), 15)
Translation_53 = model.addTranslation(Part_1_doc, [model.selection("VERTEX", "Vertex_9_1")], model.selection("EDGE", "Axis_1"), 15)
Translation_54 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_2_1")], model.selection("EDGE", "PartSet/OX"), 15)
Translation_55 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_3_1")], model.selection("EDGE", "PartSet/OX"), 0)
Translation_56 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_4_1")], model.selection("EDGE", "PartSet/OX"), -15)
Translation_57 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_5_1")], model.selection("EDGE", "InvalidName"), 15)
Translation_58 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_6_1")], model.selection("EDGE", "PartSet/OY"), "d")
Translation_59 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_7_1")], model.selection("EDGE", "Sketch_4/Edge-SketchLine_6"), 15)
Translation_60 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_8_1")], model.selection("EDGE", "Edge_9_1"), 15)
Translation_61 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_9_1")], model.selection("EDGE", "Box_1_1/Front&Box_1_1/Right"), 15)
Translation_62 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Edge_10_1")], model.selection("EDGE", "Axis_1"), 15)
Translation_63 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], model.selection("EDGE", "PartSet/OX"), 15)
Translation_64 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_2_1")], model.selection("EDGE", "PartSet/OX"), 0)
Translation_65 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_3_1")], model.selection("EDGE", "PartSet/OX"), -15)
Translation_66 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_4_1")], model.selection("EDGE", "InvalidName"), 15)
Translation_67 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_5_1")], model.selection("EDGE", "PartSet/OY"), "d")
Translation_68 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_6_1")], model.selection("EDGE", "Sketch_5/Edge-SketchLine_11"), 15)
Translation_69 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_7_1")], model.selection("EDGE", "Edge_11_1"), 15)
Translation_70 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_8_1")], model.selection("EDGE", "Box_1_1/Back&Box_1_1/Right"), 15)
Translation_71 = model.addTranslation(Part_1_doc, [model.selection("WIRE", "Wire_9_1")], model.selection("EDGE", "Axis_1"), 15)
Translation_72 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_1_1")], model.selection("EDGE", "PartSet/OX"), 15)
Translation_73 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_2_1")], model.selection("EDGE", "PartSet/OX"), 0)
Translation_74 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_3_1")], model.selection("EDGE", "PartSet/OX"), -15)
Translation_75 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_4_1")], model.selection("EDGE", "InvalidName"), -15)
Translation_76 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_5_1")], model.selection("EDGE", "PartSet/OX"), "d")
Translation_77 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_6_1")], model.selection("EDGE", "Sketch_5/Edge-SketchLine_11"), 15)
Translation_78 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_7_1")], model.selection("EDGE", "Edge_11_1"), 15)
Translation_79 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_8_1")], model.selection("EDGE", "Box_1_1/Front&Box_1_1/Right"), 15)
Translation_80 = model.addTranslation(Part_1_doc, [model.selection("FACE", "Face_9_1")], model.selection("EDGE", "Axis_1"), 15)
Translation_81 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_1_1")], model.selection("EDGE", "PartSet/OX"), 15)
Translation_82 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_2_1")], model.selection("EDGE", "PartSet/OX"), 0)
Translation_83 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_3_1")], model.selection("EDGE", "PartSet/OX"), -15)
Translation_84 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_4_1")], model.selection("EDGE", "InvalidName"), 15)
Translation_85 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_5_1")], model.selection("EDGE", "PartSet/OX"), "d")
Translation_86 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_6_1")], model.selection("EDGE", "Sketch_7/Edge-SketchLine_22"), 15)
Translation_87 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_7_1")], model.selection("EDGE", "Edge_12_1"), 15)
Translation_88 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_8_1")], model.selection("EDGE", "Box_1_1/Front&Box_1_1/Right"), 15)
Translation_89 = model.addTranslation(Part_1_doc, [model.selection("SHELL", "Shell_9_1")], model.selection("EDGE", "Axis_1"), 15)
Translation_90 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Extrusion_2_1")], model.selection("EDGE", "PartSet/OY"), 15)
model.do()
model.end()

# Checks
from GeomAPI import GeomAPI_Shape

model.testNbResults(Translation_45, 1)
model.testNbSubResults(Translation_45, [0])
model.testNbSubShapes(Translation_45, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_45, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_45, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_45, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_45, model, Part_1_doc)

model.testNbResults(Translation_46, 1)
model.testNbSubResults(Translation_46, [0])
model.testNbSubShapes(Translation_46, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_46, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_46, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_46, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_46, model, Part_1_doc)

model.testNbResults(Translation_47, 1)
model.testNbSubResults(Translation_47, [0])
model.testNbSubShapes(Translation_47, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_47, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_47, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_47, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_47, model, Part_1_doc)

model.testNbResults(Translation_49, 1)
model.testNbSubResults(Translation_49, [0])
model.testNbSubShapes(Translation_49, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_49, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_49, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_49, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_49, model, Part_1_doc)

model.testNbResults(Translation_50, 1)
model.testNbSubResults(Translation_50, [0])
model.testNbSubShapes(Translation_50, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_50, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_50, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_50, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_50, model, Part_1_doc)

model.testNbResults(Translation_51, 1)
model.testNbSubResults(Translation_51, [0])
model.testNbSubShapes(Translation_51, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_51, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_51, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_51, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_51, model, Part_1_doc)

model.testNbResults(Translation_52, 1)
model.testNbSubResults(Translation_52, [0])
model.testNbSubShapes(Translation_52, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_52, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_52, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_52, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_52, model, Part_1_doc)

model.testNbResults(Translation_53, 1)
model.testNbSubResults(Translation_53, [0])
model.testNbSubShapes(Translation_53, GeomAPI_Shape.COMPSOLID, [0])
model.testNbSubShapes(Translation_53, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_53, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_53, GeomAPI_Shape.VERTEX, [1])
model.testHaveNamingFaces(Translation_53, model, Part_1_doc)

model.testNbResults(Translation_54, 1)
model.testNbSubResults(Translation_54, [0])
model.testNbSubShapes(Translation_54, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_54, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_54, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_54, model, Part_1_doc)

model.testNbResults(Translation_55, 1)
model.testNbSubResults(Translation_55, [0])
model.testNbSubShapes(Translation_55, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_55, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_55, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_55, model, Part_1_doc)

model.testNbResults(Translation_56, 1)
model.testNbSubResults(Translation_56, [0])
model.testNbSubShapes(Translation_56, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_56, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_56, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_56, model, Part_1_doc)

model.testNbResults(Translation_58, 1)
model.testNbSubResults(Translation_58, [0])
model.testNbSubShapes(Translation_58, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_58, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_58, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_58, model, Part_1_doc)

model.testNbResults(Translation_59, 1)
model.testNbSubResults(Translation_59, [0])
model.testNbSubShapes(Translation_59, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_59, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_59, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_59, model, Part_1_doc)

model.testNbResults(Translation_60, 1)
model.testNbSubResults(Translation_60, [0])
model.testNbSubShapes(Translation_60, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_60, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_60, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_60, model, Part_1_doc)

model.testNbResults(Translation_61, 1)
model.testNbSubResults(Translation_61, [0])
model.testNbSubShapes(Translation_61, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_61, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_61, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_61, model, Part_1_doc)

model.testNbResults(Translation_62, 1)
model.testNbSubResults(Translation_62, [0])
model.testNbSubShapes(Translation_62, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_62, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_62, GeomAPI_Shape.EDGE, [1])
model.testHaveNamingFaces(Translation_62, model, Part_1_doc)

model.testNbResults(Translation_63, 1)
model.testNbSubResults(Translation_63, [0])
model.testNbSubShapes(Translation_63, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_63, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_63, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_63, model, Part_1_doc)

model.testNbResults(Translation_64, 1)
model.testNbSubResults(Translation_64, [0])
model.testNbSubShapes(Translation_64, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_64, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_64, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_64, model, Part_1_doc)

model.testNbResults(Translation_65, 1)
model.testNbSubResults(Translation_65, [0])
model.testNbSubShapes(Translation_65, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_65, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_65, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_65, model, Part_1_doc)

model.testNbResults(Translation_67, 1)
model.testNbSubResults(Translation_67, [0])
model.testNbSubShapes(Translation_67, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_67, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_67, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_67, model, Part_1_doc)

model.testNbResults(Translation_68, 1)
model.testNbSubResults(Translation_68, [0])
model.testNbSubShapes(Translation_68, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_68, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_68, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_68, model, Part_1_doc)

model.testNbResults(Translation_69, 1)
model.testNbSubResults(Translation_69, [0])
model.testNbSubShapes(Translation_69, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_69, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_69, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_69, model, Part_1_doc)

model.testNbResults(Translation_70, 1)
model.testNbSubResults(Translation_70, [0])
model.testNbSubShapes(Translation_70, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_70, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_70, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_70, model, Part_1_doc)

model.testNbResults(Translation_71, 1)
model.testNbSubResults(Translation_71, [0])
model.testNbSubShapes(Translation_71, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_71, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_71, GeomAPI_Shape.WIRE, [1])
model.testHaveNamingFaces(Translation_71, model, Part_1_doc)

model.testNbResults(Translation_72, 1)
model.testNbSubResults(Translation_72, [0])
model.testNbSubShapes(Translation_72, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_72, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_72, model, Part_1_doc)

model.testNbResults(Translation_73, 1)
model.testNbSubResults(Translation_73, [0])
model.testNbSubShapes(Translation_73, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_73, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_73, model, Part_1_doc)

model.testNbResults(Translation_74, 1)
model.testNbSubResults(Translation_74, [0])
model.testNbSubShapes(Translation_74, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_74, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_74, model, Part_1_doc)

model.testNbResults(Translation_76, 1)
model.testNbSubResults(Translation_76, [0])
model.testNbSubShapes(Translation_76, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_76, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_76, model, Part_1_doc)

model.testNbResults(Translation_77, 1)
model.testNbSubResults(Translation_77, [0])
model.testNbSubShapes(Translation_77, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_77, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_77, model, Part_1_doc)

model.testNbResults(Translation_78, 1)
model.testNbSubResults(Translation_78, [0])
model.testNbSubShapes(Translation_78, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_78, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_78, model, Part_1_doc)

model.testNbResults(Translation_79, 1)
model.testNbSubResults(Translation_79, [0])
model.testNbSubShapes(Translation_79, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_79, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_79, model, Part_1_doc)

model.testNbResults(Translation_80, 1)
model.testNbSubResults(Translation_80, [0])
model.testNbSubShapes(Translation_80, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_80, GeomAPI_Shape.FACE, [1])
model.testHaveNamingFaces(Translation_80, model, Part_1_doc)

model.testNbResults(Translation_81, 1)
model.testNbSubResults(Translation_81, [0])
model.testNbSubShapes(Translation_81, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_81, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_81, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_82, 1)
model.testNbSubResults(Translation_82, [0])
model.testNbSubShapes(Translation_82, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_82, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_82, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_83, 1)
model.testNbSubResults(Translation_83, [0])
model.testNbSubShapes(Translation_83, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_83, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_83, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_85, 1)
model.testNbSubResults(Translation_85, [0])
model.testNbSubShapes(Translation_85, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_85, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_85, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_86, 1)
model.testNbSubResults(Translation_86, [0])
model.testNbSubShapes(Translation_86, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_86, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_86, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_87, 1)
model.testNbSubResults(Translation_87, [0])
model.testNbSubShapes(Translation_87, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_87, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_87, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_88, 1)
model.testNbSubResults(Translation_88, [0])
model.testNbSubShapes(Translation_88, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_88, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_88, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_89, 1)
model.testNbSubResults(Translation_89, [0])
model.testNbSubShapes(Translation_89, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_89, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Translation_89, GeomAPI_Shape.SHELL, [1])

model.testNbResults(Translation_90, 2)
model.testNbSubResults(Translation_90, [0, 0])
model.testNbSubShapes(Translation_90, GeomAPI_Shape.SOLID, [1, 1])
model.testNbSubShapes(Translation_90, GeomAPI_Shape.FACE, [6, 3])
model.testHaveNamingFaces(Translation_90, model, Part_1_doc)


model.testNbResults(Translation_48, 0)
assert(Translation_48.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Translation_57, 0)
assert(Translation_57.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Translation_66, 0)
assert(Translation_66.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Translation_75, 0)
assert(Translation_75.feature().error() == 'Attribute "axis_object" is not initialized.')

model.testNbResults(Translation_84, 0)
assert(Translation_84.feature().error() == 'Attribute "axis_object" is not initialized.')
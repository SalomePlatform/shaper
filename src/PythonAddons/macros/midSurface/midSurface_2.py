#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Géométrie de test pour la création de surfaces médianes - Cas n°2
"""

__revision__ = "V02.01"

nom_objet = "meli-melo"
fichier = "midSurface_2.stp"

import salome

salome.salome_init()

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
model.addParameter(partSet, "H", '24')
model.addParameter(partSet, "L", '36')
model.addParameter(partSet, "h", '2')
model.addParameter(partSet, "theta_1", '45')
model.addParameter(partSet, "C_x", '30')
model.addParameter(partSet, "DX", '90')
model.addParameter(partSet, "C_y", '40')
model.addParameter(partSet, "R_int", '50', 'Rayon interne')
model.addParameter(partSet, "R_ext", '51', 'Rayon externe')
model.addParameter(partSet, "R_1", '15')
model.addParameter(partSet, "R_2", '20')
model.addParameter(partSet, "R_3", '5')
model.addParameter(partSet, "theta_2", '60')
model.addParameter(partSet, "B_1", '6')
model.addParameter(partSet, "Ep", '0.5', 'Epaisseur')
model.addParameter(partSet, "Hauteur", '150.', 'Hauteur')
model.addParameter(partSet, "coeff", '0.00001', 'Coefficient')

# 1. Deux solides plans minces
#=============================

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 0, 48, 0)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates())

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(48, 0, 103.7207793864215, 55.72077938642148)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(103.7207793864215, 55.72077938642148, 78.26493526370577, 81.17662350913719)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(78.26493526370577, 81.17662350913719, 0, 72)
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(0, 72, 0, 0)
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.endPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_5.result())
Sketch_1.setLength(SketchLine_5.result(), "2*L")
Sketch_1.setLength(SketchLine_1.result(), "2*H")
Sketch_1.setEqual(SketchLine_2.result(), SketchLine_4.result())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_6 = SketchProjection_2.createdFeature()

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_6.result(), SketchLine_2.result(), "theta_1", type = "Direct")
Sketch_1.setPerpendicular(SketchLine_2.result(), SketchLine_3.result())
Sketch_1.setLength(SketchLine_3.result(), "L")

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(0, 0, -96, 0)
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_7.startPoint())
Sketch_1.setHorizontal(SketchLine_7.result())
Sketch_1.setLength(SketchLine_7.result(), "4*H")

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(-96, 4.800000000000001, -4.800000000000001, 4.800000000000001)
Sketch_1.setHorizontal(SketchLine_8.result())

### Create SketchLine
SketchLine_9 = Sketch_1.addLine(-4.800000000000001, 4.800000000000001, -4.800000000000001, 72)
Sketch_1.setVertical(SketchLine_9.result())
Sketch_1.setCoincident(SketchLine_8.endPoint(), SketchLine_9.startPoint())

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(-4.800000000000001, 72, 0, 72)
Sketch_1.setCoincident(SketchLine_9.endPoint(), SketchLine_10.startPoint())
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_10.endPoint())
Sketch_1.setLength(SketchLine_10.result(), "0.2*H")
Sketch_1.setHorizontal(SketchLine_10.result())

### Create SketchLine
SketchLine_11 = Sketch_1.addLine(-96, 4.800000000000001, -96, 0)
Sketch_1.setCoincident(SketchLine_8.startPoint(), SketchLine_11.startPoint())
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_11.endPoint())
Sketch_1.setVertical(SketchLine_11.result())
Sketch_1.setLength(SketchLine_11.result(), "0.2*H")
model.do()
Sketch_1.setName("Objet_2_Sketch")
Sketch_1.result().setName("Objet_2_Sketch")

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Objet_2_Sketch")], model.selection(), "h", 0, "Faces|Wires")
Extrusion_1.setName("Objet_2")
Extrusion_1.result().setName("Objet_2")
Extrusion_1.result().subResult(0).setName("Extrusion_1_1_1")
Extrusion_1.result().subResult(1).setName("Extrusion_1_1_2")
model.do()

# 2. Un cylindre découpé
#=======================

### Create Part
Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()

### Create Sketch
Sketch_2 = model.addSketch(Part_2_doc, model.standardPlane("YOZ"))

### Create SketchCircle
SketchCircle_1 = Sketch_2.addCircle(30, 40, 50)
Sketch_2.setRadius(SketchCircle_1.results()[1], "R_int")

### Create SketchProjection
SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_3.createdFeature()
Sketch_2.setHorizontalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_2).coordinates(), "C_x")
Sketch_2.setVerticalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_2).coordinates(), "C_y")

### Create SketchCircle
SketchCircle_2 = Sketch_2.addCircle(30, 40, 51)
Sketch_2.setRadius(SketchCircle_2.results()[1], "R_ext")
Sketch_2.setCoincident(SketchCircle_2.center(), SketchCircle_1.center())
model.do()
Sketch_2.setName("Objet_6_base_Sketch")
Sketch_2.result().setName("Objet_6_base_Sketch")

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_2_doc, [model.selection("COMPOUND", "all-in-Objet_6_base_Sketch")], model.selection(), "DX", 0, "Faces|Wires")
Extrusion_2.setName("Objet_6_base_Extrusion")
Extrusion_2.result().setName("Objet_6_base_Extrusion")
Extrusion_2.result().subResult(0).setName("Objet_6_base_Extrusion_1")
Extrusion_2.result().subResult(1).setName("Objet_6_base_Extrusion_2")

### Create Remove_SubShapes
Remove_SubShapes_1 = model.addRemoveSubShapes(Part_2_doc, model.selection("COMPSOLID", "Objet_6_base_Extrusion"))
Remove_SubShapes_1.setSubShapesToKeep([model.selection("SOLID", "Objet_6_base_Extrusion_2")])
Remove_SubShapes_1.setName("Objet_6_base_RemoveSubShapes")
Remove_SubShapes_1.result().setName("Objet_6_base")

### Create Translation
Translation_1 = model.addTranslation(Part_2_doc, [model.selection("SOLID", "Objet_6_base")], vector = ["H", "L", "H+L"], keepSubResults = True)
Translation_1.setName("Objet_6_Translation")
Translation_1.result().setName("Objet_6_Translation")

### Create Point
Point_2 = model.addPoint(Part_2_doc, 0, 0, 0)
Point_2.setName("Objet_6_Point_1")
Point_2.result().setName("Objet_6_Point_1")

### Create Point
Point_3 = model.addPoint(Part_2_doc, 1, 2, 3)
Point_3.setName("Objet_6_Point_2")
Point_3.result().setName("Objet_6_Point_2")

### Create Axis
Axis_4 = model.addAxis(Part_2_doc, model.selection("VERTEX", "Objet_6_Point_1"), model.selection("VERTEX", "Objet_6_Point_2"))
Axis_4.setName("Objet_6_Axe")
Axis_4.result().setName("Objet_6_Axe")

### Create Rotation
Rotation_1 = model.addRotation(Part_2_doc, [model.selection("SOLID", "Objet_6_Translation")], axis = model.selection("EDGE", "Objet_6_Axe"), angle = 30, keepSubResults = True)
Rotation_1.setName("Objet_6_Rotation")
Rotation_1.result().setName("Objet_6_Rotation")

### Create Sketch
Sketch_3 = model.addSketch(Part_2_doc, model.standardPlane("XOY"))

### Create SketchLine
SketchLine_12 = Sketch_3.addLine(36, 120, 59.99999999999999, 78.43078061834694)

### Create SketchLine
SketchLine_13 = Sketch_3.addLine(59.99999999999999, 78.43078061834694, 80.78460969082653, 90.43078061834694)
Sketch_3.setCoincident(SketchLine_12.endPoint(), SketchLine_13.startPoint())

### Create SketchLine
SketchLine_14 = Sketch_3.addLine(80.78460969082653, 90.43078061834694, 104.7846096908265, 48.86156123669388)
Sketch_3.setCoincident(SketchLine_13.endPoint(), SketchLine_14.startPoint())

### Create SketchLine
SketchLine_15 = Sketch_3.addLine(104.7846096908265, 48.86156123669388, 60.37400840781982, 30.64896961216652)
Sketch_3.setCoincident(SketchLine_14.endPoint(), SketchLine_15.startPoint())

### Create SketchLine
SketchLine_16 = Sketch_3.addLine(60.37400840781982, 30.64896961216652, 36, 72)
Sketch_3.setCoincident(SketchLine_15.endPoint(), SketchLine_16.startPoint())

### Create SketchLine
SketchLine_17 = Sketch_3.addLine(36, 72, 36, 120)
Sketch_3.setCoincident(SketchLine_16.endPoint(), SketchLine_17.startPoint())
Sketch_3.setCoincident(SketchLine_12.startPoint(), SketchLine_17.endPoint())
Sketch_3.setVertical(SketchLine_17.result())
Sketch_3.setEqual(SketchLine_17.result(), SketchLine_12.result())
Sketch_3.setEqual(SketchLine_12.result(), SketchLine_15.result())
Sketch_3.setPerpendicular(SketchLine_13.result(), SketchLine_14.result())
Sketch_3.setPerpendicular(SketchLine_13.result(), SketchLine_12.result())
Sketch_3.setEqual(SketchLine_14.result(), SketchLine_16.result())

### Create SketchProjection
SketchProjection_4 = Sketch_3.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_4.createdFeature()
Sketch_3.setHorizontalDistance(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_16.endPoint(), "L")
Sketch_3.setVerticalDistance(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_16.endPoint(), "2*L")
Sketch_3.setLength(SketchLine_14.result(), "2.*H")
Sketch_3.setLength(SketchLine_13.result(), "H")

### Create SketchConstraintAngle
Sketch_3.setAngle(SketchLine_17.result(), SketchLine_12.result(), 30, type = "Direct")
Sketch_3.setLength(SketchLine_15.result(), "2.*H")
model.do()
Sketch_3.setName("Objet_6_Sketch_bloc_a")
Sketch_3.result().setName("Objet_6_Sketch_bloc_a")

### Create Extrusion
Extrusion_3 = model.addExtrusion(Part_2_doc, [model.selection("COMPOUND", "Objet_6_Sketch_bloc_a")], model.selection(), 200, 100, "Faces|Wires")
Extrusion_3.setName("Objet_6_a")
Extrusion_3.result().setName("Objet_6_a")

### Create Common
Common_1 = model.addCommon(Part_2_doc, [model.selection("SOLID", "Objet_6_Rotation"), model.selection("SOLID", "Objet_6_a")], keepSubResults = True)
Common_1.setName("Objet_6")
Common_1.result().setName("Objet_6")
Common_1.result().subResult(0).setName("Common_1_1_1")
Common_1.result().subResult(1).setName("Common_1_1_2")
Folder_1 = model.addFolder(Part_2_doc, Sketch_2, Extrusion_3)
Folder_1.setName("Objet_6_création")

model.do()

# 3. Un tore découpé
#===================

### Create Part
Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()

### Create Torus
Torus_1 = model.addTorus(Part_3_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R_2", "R_3")
Torus_1.setName("Objet_8_Tore_1")
Torus_1.result().setName("Objet_8_Tore_1")

### Create Torus
Torus_2 = model.addTorus(Part_3_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R_2", "R_3-h")
Torus_2.setName("Objet_8_Tore_2")
Torus_2.result().setName("Objet_8_Tore_2")

### Create Cut
Cut_1 = model.addCut(Part_3_doc, [model.selection("SOLID", "Objet_8_Tore_1")], [model.selection("SOLID", "Objet_8_Tore_2")], keepSubResults = True)
Cut_1.setName("Objet_8_Cut")
Cut_1.result().setName("Objet_8_Cut")

### Create Sketch
Sketch_4 = model.addSketch(Part_3_doc, model.standardPlane("XOY"))

### Create SketchLine
SketchLine_18 = Sketch_4.addLine(0, 33.9282032302755, 6, 33.9282032302755)
Sketch_4.setHorizontal(SketchLine_18.result())

### Create SketchLine
SketchLine_19 = Sketch_4.addLine(6, 33.9282032302755, 6, 5)
Sketch_4.setCoincident(SketchLine_18.endPoint(), SketchLine_19.startPoint())
Sketch_4.setVertical(SketchLine_19.result())

### Create SketchLine
SketchLine_20 = Sketch_4.addLine(6, 5, 31.05255888325765, -9.464101615137745)
Sketch_4.setCoincident(SketchLine_19.endPoint(), SketchLine_20.startPoint())

### Create SketchLine
SketchLine_21 = Sketch_4.addLine(31.05255888325765, -9.464101615137745, 25.05255888325765, -19.85640646055101)
Sketch_4.setCoincident(SketchLine_20.endPoint(), SketchLine_21.startPoint())

### Create SketchLine
SketchLine_22 = Sketch_4.addLine(25.05255888325765, -19.85640646055101, 6, -8.856406460550954)
Sketch_4.setCoincident(SketchLine_21.endPoint(), SketchLine_22.startPoint())

### Create SketchLine
SketchLine_23 = Sketch_4.addLine(6, -8.856406460550954, 6, -28.8564064605506)
Sketch_4.setCoincident(SketchLine_22.endPoint(), SketchLine_23.startPoint())
Sketch_4.setVertical(SketchLine_23.result())

### Create SketchLine
SketchLine_24 = Sketch_4.addLine(6, -28.8564064605506, 0, -28.8564064605506)
Sketch_4.setCoincident(SketchLine_23.endPoint(), SketchLine_24.startPoint())
Sketch_4.setHorizontal(SketchLine_24.result())

### Create SketchProjection
SketchProjection_5 = Sketch_4.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_25 = SketchProjection_5.createdFeature()
Sketch_4.setCoincident(SketchLine_18.startPoint(), SketchLine_25.result())
Sketch_4.setCoincident(SketchLine_24.endPoint(), SketchLine_25.result())
Sketch_4.setEqual(SketchLine_18.result(), SketchLine_24.result())
Sketch_4.setPerpendicular(SketchLine_20.result(), SketchLine_21.result())
Sketch_4.setPerpendicular(SketchLine_22.result(), SketchLine_21.result())

### Create SketchConstraintAngle
Sketch_4.setAngle(SketchLine_23.result(), SketchLine_22.result(), "theta_2", type = "Direct")
Sketch_4.setLength(SketchLine_18.result(), "B_1")
Sketch_4.setLength(SketchLine_21.result(), "2.*B_1")

### Create SketchProjection
SketchProjection_6 = Sketch_4.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_4 = SketchProjection_6.createdFeature()
Sketch_4.setVerticalDistance(SketchLine_20.startPoint(), SketchAPI_Point(SketchPoint_4).coordinates(), "R_2/4.")
Sketch_4.setLength(SketchLine_22.result(), "1.1*R_2")
Sketch_4.setLength(SketchLine_23.result(), "R_2")
Sketch_4.setEqual(SketchLine_20.result(), SketchLine_19.result())

### Create SketchConstraintMirror
SketchConstraintMirror_1_objects = [SketchLine_24.result(), SketchLine_23.result(), SketchLine_18.result(), SketchLine_19.result(), SketchLine_21.result(), SketchLine_20.result(), SketchLine_22.result()]
SketchConstraintMirror_1 = Sketch_4.addMirror(SketchLine_25.result(), SketchConstraintMirror_1_objects)
[SketchLine_26, SketchLine_27, SketchLine_28, SketchLine_29, SketchLine_30, SketchLine_31, SketchLine_32] = SketchConstraintMirror_1.mirrored()
model.do()
Sketch_4.setName("Objet_8_Sketch_bloc_b")
Sketch_4.result().setName("Objet_8_Sketch_bloc_b")

### Create Extrusion
Extrusion_4 = model.addExtrusion(Part_3_doc, [model.selection("COMPOUND", "Objet_8_Sketch_bloc_b")], model.selection(), "4.*R_3", "4.*R_3", "Faces|Wires")
Extrusion_4.setName("Objet_8_b")
Extrusion_4.result().setName("Objet_8_b")

### Create Common
Common_2 = model.addCommon(Part_3_doc, [model.selection("SOLID", "Objet_8_Cut"), model.selection("SOLID", "Objet_8_b")], keepSubResults = True)
Common_2.setName("Objet_8_Common")
Common_2.result().setName("Objet_8_Common")
Common_2.result().subResult(0).setName("Common_1_1_1")
Common_2.result().subResult(1).setName("Common_1_1_2")
Common_2.result().subResult(2).setName("Common_1_1_3")
Common_2.result().subResult(3).setName("Common_1_1_4")

### Create Point
Point_4 = model.addPoint(Part_3_doc, -1, -2, -3)
Point_4.setName("Objet_8_Point_1")
Point_4.result().setName("Objet_8_Point_1")

### Create Point
Point_5 = model.addPoint(Part_3_doc, 20, 30, 15)
Point_5.setName("Objet_8_Point_2")
Point_5.result().setName("Objet_8_Point_2")

### Create Axis
Axis_5 = model.addAxis(Part_3_doc, model.selection("VERTEX", "Objet_8_Point_1"), model.selection("VERTEX", "Objet_8_Point_2"))
Axis_5.setName("Objet_8_Axe")
Axis_5.result().setName("Objet_8_Axe")

### Create Rotation
Rotation_2 = model.addRotation(Part_3_doc, [model.selection("COMPOUND", "Objet_8_Common")], axis = model.selection("EDGE", "Objet_8_Axe"), angle = 45, keepSubResults = True)
Rotation_2.setName("Objet_8")
Rotation_2.result().setName("Objet_8")
Rotation_2.result().subResult(0).setName("Rotation_1_1_1")
Rotation_2.result().subResult(1).setName("Rotation_1_1_2")
Rotation_2.result().subResult(2).setName("Rotation_1_1_3")
Rotation_2.result().subResult(3).setName("Rotation_1_1_4")
Folder_2 = model.addFolder(Part_3_doc, Torus_1, Axis_5)
Folder_2.setName("Objet_8_création")

model.do()

# 4. Un cône découpé
#===================

### Create Part
Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()

### Create Cone
Cone_1 = model.addCone(Part_4_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R_2", "R_3", "DX")
Cone_1.setName("Objet_9_Cone_1")
Cone_1.result().setName("Objet_9_Cone_1")

### Create Cone
Cone_2 = model.addCone(Part_4_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), "R_2-h", "R_3-h", "DX")
Cone_2.setName("Objet_9_Cone_2")
Cone_2.result().setName("Objet_9_Cone_2")

### Create Cut
Cut_2 = model.addCut(Part_4_doc, [model.selection("SOLID", "Objet_9_Cone_1")], [model.selection("SOLID", "Objet_9_Cone_2")], keepSubResults = True)
Cut_2.setName("Objet_9_Cut")
Cut_2.result().setName("Objet_9_Cut")

### Create Sketch
Sketch_5 = model.addSketch(Part_4_doc, model.standardPlane("XOY"))

### Create SketchLine
SketchLine_33 = Sketch_5.addLine(0, 33.9282032302755, 6, 33.9282032302755)
Sketch_5.setHorizontal(SketchLine_33.result())

### Create SketchLine
SketchLine_34 = Sketch_5.addLine(6, 33.9282032302755, 6, 5)
Sketch_5.setCoincident(SketchLine_33.endPoint(), SketchLine_34.startPoint())
Sketch_5.setVertical(SketchLine_34.result())

### Create SketchLine
SketchLine_35 = Sketch_5.addLine(6, 5, 31.05255888325765, -9.464101615137745)
Sketch_5.setCoincident(SketchLine_34.endPoint(), SketchLine_35.startPoint())

### Create SketchLine
SketchLine_36 = Sketch_5.addLine(31.05255888325765, -9.464101615137745, 25.05255888325765, -19.85640646055101)
Sketch_5.setCoincident(SketchLine_35.endPoint(), SketchLine_36.startPoint())

### Create SketchLine
SketchLine_37 = Sketch_5.addLine(25.05255888325765, -19.85640646055101, 6, -8.856406460550954)
Sketch_5.setCoincident(SketchLine_36.endPoint(), SketchLine_37.startPoint())

### Create SketchLine
SketchLine_38 = Sketch_5.addLine(6, -8.856406460550954, 6, -28.8564064605506)
Sketch_5.setCoincident(SketchLine_37.endPoint(), SketchLine_38.startPoint())
Sketch_5.setVertical(SketchLine_38.result())

### Create SketchLine
SketchLine_39 = Sketch_5.addLine(6, -28.8564064605506, 0, -28.8564064605506)
Sketch_5.setCoincident(SketchLine_38.endPoint(), SketchLine_39.startPoint())
Sketch_5.setHorizontal(SketchLine_39.result())

### Create SketchProjection
SketchProjection_7 = Sketch_5.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_40 = SketchProjection_7.createdFeature()
Sketch_5.setCoincident(SketchLine_33.startPoint(), SketchLine_40.result())
Sketch_5.setCoincident(SketchLine_39.endPoint(), SketchLine_40.result())
Sketch_5.setEqual(SketchLine_33.result(), SketchLine_39.result())
Sketch_5.setPerpendicular(SketchLine_35.result(), SketchLine_36.result())
Sketch_5.setPerpendicular(SketchLine_37.result(), SketchLine_36.result())

### Create SketchConstraintAngle
Sketch_5.setAngle(SketchLine_38.result(), SketchLine_37.result(), "theta_2", type = "Direct")
Sketch_5.setLength(SketchLine_33.result(), "B_1")
Sketch_5.setLength(SketchLine_36.result(), "2.*B_1")

### Create SketchProjection
SketchProjection_8 = Sketch_5.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_5 = SketchProjection_8.createdFeature()
Sketch_5.setVerticalDistance(SketchLine_35.startPoint(), SketchAPI_Point(SketchPoint_5).coordinates(), "R_2/4.")
Sketch_5.setLength(SketchLine_37.result(), "1.1*R_2")
Sketch_5.setLength(SketchLine_38.result(), "R_2")
Sketch_5.setEqual(SketchLine_35.result(), SketchLine_34.result())

### Create SketchConstraintMirror
SketchConstraintMirror_2_objects = [SketchLine_39.result(), SketchLine_38.result(), SketchLine_33.result(), SketchLine_34.result(), SketchLine_36.result(), SketchLine_35.result(), SketchLine_37.result()]
SketchConstraintMirror_2 = Sketch_5.addMirror(SketchLine_40.result(), SketchConstraintMirror_2_objects)
[SketchLine_41, SketchLine_42, SketchLine_43, SketchLine_44, SketchLine_45, SketchLine_46, SketchLine_47] = SketchConstraintMirror_2.mirrored()
model.do()
Sketch_5.setName("Objet_9_Sketch_bloc_b")
Sketch_5.result().setName("Objet_9_Sketch_bloc_b")

### Create Extrusion
Extrusion_5 = model.addExtrusion(Part_4_doc, [model.selection("COMPOUND", "Objet_9_Sketch_bloc_b")], model.selection(), "4.*R_3", "4.*R_3", "Faces|Wires")
Extrusion_5.setName("Objet_9_b")
Extrusion_5.result().setName("Objet_9_b")

### Create Rotation
Rotation_3 = model.addRotation(Part_4_doc, [model.selection("SOLID", "Objet_9_b")], axis = model.selection("EDGE", "PartSet/OX"), angle = 10, keepSubResults = True)
Rotation_3.setName("Objet_9_b_1")
Rotation_3.result().setName("Objet_9_b_1")

### Create Common
Common_3 = model.addCommon(Part_4_doc, [model.selection("SOLID", "Objet_9_Cut"), model.selection("SOLID", "Objet_9_b_1")], keepSubResults = True)
Common_3.setName("Objet_9_Common")
Common_3.result().setName("Objet_9_Common")
Common_3.result().subResult(0).setName("Common_1_1_1")
Common_3.result().subResult(1).setName("Common_1_1_2")
Common_3.result().subResult(2).setName("Common_1_1_3")
Common_3.result().subResult(3).setName("Common_1_1_4")

### Create Translation
Translation_2 = model.addTranslation(Part_4_doc, [model.selection("COMPOUND", "all-in-Objet_9_Common")], axis = model.selection("EDGE", "PartSet/OZ"), distance = "-2*R_2", keepSubResults = True)
Translation_2.setName("Objet_9_Common")
Translation_2.result().setName("Objet_9_Common")
Translation_2.result().subResult(0).setName("Translation_1_1_1")
Translation_2.result().subResult(0).subResult(0).setName("Translation_1_1_1_1")
Translation_2.result().subResult(0).subResult(1).setName("Translation_1_1_1_2")
Translation_2.result().subResult(0).subResult(2).setName("Translation_1_1_1_3")
Translation_2.result().subResult(0).subResult(3).setName("Translation_1_1_1_4")

### Create Point
Point_6 = model.addPoint(Part_4_doc, -1, -2, -3)
Point_6.setName("Objet_9_Point_1")
Point_6.result().setName("Objet_9_Point_1")

### Create Point
Point_7 = model.addPoint(Part_4_doc, 20, 30, 15)
Point_7.setName("Objet_9_Point_2")
Point_7.result().setName("Objet_9_Point_2")

### Create Axis
Axis_6 = model.addAxis(Part_4_doc, model.selection("VERTEX", "Objet_9_Point_1"), model.selection("VERTEX", "Objet_9_Point_2"))
Axis_6.setName("Objet_9_Axe")
Axis_6.result().setName("Objet_9_Axe")

### Create Rotation
Rotation_4 = model.addRotation(Part_4_doc, [model.selection("COMPOUND", "Objet_9_Common")], axis = model.selection("EDGE", "Objet_9_Axe"), angle = 45, keepSubResults = True)
Rotation_4.setName("Objet_9")
Rotation_4.result().setName("Objet_9")
Rotation_4.result().subResult(0).setName("Rotation_2_1_1")
Rotation_4.result().subResult(0).subResult(0).setName("Rotation_2_1_1_1")
Rotation_4.result().subResult(0).subResult(1).setName("Rotation_2_1_1_2")
Rotation_4.result().subResult(0).subResult(2).setName("Rotation_2_1_1_3")
Rotation_4.result().subResult(0).subResult(3).setName("Rotation_2_1_1_4")
Folder_3 = model.addFolder(Part_4_doc, Cone_1, Axis_6)
Folder_3.setName("Objet_9_création")

model.do()

# 5. Assemblage des composants
#=============================

### Create Part
Part_5 = model.addPart(partSet)
Part_5_doc = Part_5.document()

### Create ImportResult
ImportResult_1_objects = [model.selection("COMPOUND", "Part_4/Objet_9"),
                          model.selection("COMPOUND", "Part_3/Objet_8"),
                          model.selection("COMPOUND", "Part_2/Objet_6"),
                          model.selection("COMPSOLID", "Part_1/Objet_2")]
ImportResult_1 = model.addImportResult(Part_5_doc, ImportResult_1_objects)

### Create Partition
Partition_1_objects = [model.selection("COMPOUND", "ImportResult_1_1"),
                       model.selection("COMPOUND", "ImportResult_1_2"),
                       model.selection("COMPOUND", "ImportResult_1_3"),
                       model.selection("COMPSOLID", "ImportResult_1_4")]
Partition_1 = model.addPartition(Part_5_doc, Partition_1_objects, keepSubResults = True)
Partition_1.result().setName(nom_objet)

### Exportation
print ("Exportation dans {}".format(fichier))
export = model.exportToFile(Part_5_doc, fichier, [model.selection("COMPSOLID", nom_objet)])

model.end()

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()

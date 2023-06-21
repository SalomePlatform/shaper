#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""Géométrie de test pour la création de surfaces médianes - Cas n°1
"""

__revision__ = "V02.01"

nom_objet = "Observatoire"
fichier = "midSurface_1.stp"

import salome

salome.salome_init()

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "R_ext", "51", "Rayon externe")
model.addParameter(Part_1_doc, "Ep", "0.5", "Epaisseur")
model.addParameter(Part_1_doc, "Hauteur", "150", "Hauteur")

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(0, 0, 51, 0)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(51, 0, 51, 150)
Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_1.endPoint())

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_3 = SketchProjection_1.createdFeature()
Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_3.result())
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setLength(SketchLine_2.result(), "Hauteur")
Sketch_1.setCoincident(SketchAPI_Line(SketchLine_3).startPoint(), SketchLine_1.startPoint())

### Create SketchProjection
SketchProjection_2 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
Sketch_1.setHorizontalDistance(SketchLine_2.startPoint(), SketchAPI_Point(SketchPoint_1).coordinates(), "R_ext")

### Create SketchProjection
SketchProjection_3 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_4 = SketchProjection_3.createdFeature()

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(0, 150, 51, 150, 0, 201, False)
Sketch_1.setCoincident(SketchLine_4.result(), SketchArc_1.center())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
Sketch_1.setCoincident(SketchLine_4.result(), SketchArc_1.endPoint())

### Create SketchProjection
SketchProjection_4 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_4.createdFeature()
Sketch_1.setVerticalDistance(SketchArc_1.center(), SketchAPI_Point(SketchPoint_2).coordinates(), "Hauteur")

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(0, 0, 0, 0.5)
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(0, 0.5, 50.5, 0.5)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_6.startPoint())
Sketch_1.setHorizontal(SketchLine_6.result())

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(50.5, 0.5, 50.5, 150)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_7.startPoint())
Sketch_1.setVertical(SketchLine_7.result())
Sketch_1.setVertical(SketchLine_5.result())
Sketch_1.setLength(SketchLine_5.result(), "Ep")
Sketch_1.setHorizontalDistance(SketchLine_2.endPoint(), SketchLine_7.endPoint(), "Ep")

### Create SketchLine
SketchLine_8 = Sketch_1.addLine(50.5, 150, 51, 150)
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchLine_8.startPoint())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchLine_8.endPoint())
Sketch_1.setHorizontal(SketchLine_8.result())

### Create SketchArc
SketchArc_2 = Sketch_1.addArc(0, 150, 50.5, 150, 0, 200.5, False)
Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
Sketch_1.setCoincident(SketchLine_7.endPoint(), SketchArc_2.startPoint())
Sketch_1.setCoincident(SketchLine_4.result(), SketchArc_2.endPoint())

### Create SketchLine
SketchLine_9 = Sketch_1.addLine(0, 201, 0, 200.5)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_9.startPoint())
Sketch_1.setCoincident(SketchArc_2.endPoint(), SketchLine_9.endPoint())

### Create SketchLine
SketchLine_10 = Sketch_1.addLine(50.5, 0.5, 51, 0.5)
Sketch_1.setCoincident(SketchLine_6.endPoint(), SketchLine_10.startPoint())
Sketch_1.setCoincident(SketchLine_10.endPoint(), SketchLine_2.result())
Sketch_1.setHorizontal(SketchLine_10.result())
model.do()

### Create Revolution
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection("EDGE", "PartSet/OZ"), 360, 0, "Faces")

### Create Box
Box_1 = model.addBox(Part_1_doc, "R_ext*1.2", "R_ext*.2", "Hauteur*0.5")

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Box_1")], vector = ["R_ext*0.15", "-R_ext*0.1", "Hauteur*0.9"], keepSubResults = True)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Revolution_1")], [model.selection("COMPOUND", "all-in-Translation_1")], keepSubResults = True)
Cut_1.result().setName(nom_objet)

### Exportation
print ("Exportation dans {}".format(fichier))
export = model.exportToFile(Part_1_doc, fichier, [model.selection("COMPSOLID", nom_objet)])

model.end()

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()

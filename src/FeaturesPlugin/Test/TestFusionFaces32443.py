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

from salome.shaper import model
from SketchAPI import *

# Create document
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

model.addParameter(Part_1_doc, "hauteur", '200')
model.addParameter(Part_1_doc, "rayon", '20')
model.addParameter(Part_1_doc, "largeur", '100')
model.addParameter(Part_1_doc, "epaisseur", '10')
model.addParameter(Part_1_doc, "distance", '50')

Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchLine_2 = Sketch_1.addLine(0, 50, 0, 60)
SketchLine_3 = Sketch_1.addLine(100, 60.00000000000041, 100, 50.00000000001594)
Sketch_1.setVertical(SketchLine_2.result())
Sketch_1.setVertical(SketchLine_3.result())
Sketch_1.setVerticalDistance(SketchLine_2.endPoint(), SketchLine_2.startPoint(), "epaisseur")
Sketch_1.setVerticalDistance(SketchAPI_Line(SketchLine_1).startPoint(), SketchLine_2.startPoint(), "distance")

SketchProjection_2 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OX"), False)
SketchLine_4 = SketchProjection_2.createdFeature()

SketchArc_1 = Sketch_1.addArc(50, 0, 0, 60, 100, 60.00000000000041, True)
Sketch_1.setCoincident(SketchLine_4.result(), SketchArc_1.center())
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
Sketch_1.setCoincident(SketchLine_3.result(), SketchArc_1.endPoint())

SketchArc_2 = Sketch_1.addArc(50, 0, 0, 50, 100, 50.00000000001594, True)
Sketch_1.setCoincident(SketchArc_1.center(), SketchArc_2.center())
Sketch_1.setCoincident(SketchLine_3.result(), SketchArc_2.endPoint())
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchArc_2.endPoint())
Sketch_1.setCoincident(SketchArc_2.startPoint(), SketchLine_2.startPoint())
Sketch_1.setCoincident(SketchLine_2.startPoint(), SketchLine_1.result())
Sketch_1.setHorizontalDistance(SketchArc_2.center(), SketchAPI_Line(SketchLine_1).startPoint(), "largeur/2")
Sketch_1.setHorizontalDistance(SketchArc_2.endPoint(), SketchAPI_Line(SketchLine_1).startPoint(), "largeur")
model.do()

Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), "hauteur", 0, "Faces|Wires")

Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchCircle_1 = Sketch_2.addCircle(50, 100, 20)

SketchProjection_3 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_3.createdFeature()
Sketch_2.setHorizontalDistance(SketchAPI_Point(SketchPoint_1).coordinates(), SketchCircle_1.center(), "largeur/2")
Sketch_2.setVerticalDistance(SketchCircle_1.center(), SketchAPI_Point(SketchPoint_1).coordinates(), "hauteur/2")
Sketch_2.setRadius(SketchCircle_1.results()[1], "rayon")
model.do()

ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), [model.selection("COMPOUND", "all-in-Extrusion_1")])

Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOZ"))
SketchLine_5 = Sketch_3.addLine(0, 150, 100, 50)
SketchLine_6 = Sketch_3.addLine(0, 50, 100, 150)
SketchProjection_4 = Sketch_3.addProjection(model.selection("EDGE", "PartSet/OZ"), False)
SketchLine_7 = SketchProjection_4.createdFeature()
Sketch_3.setCoincident(SketchLine_6.startPoint(), SketchLine_7.result())
SketchLine_8 = Sketch_3.addLine(0, 150, 100, 150)
Sketch_3.setCoincident(SketchLine_5.startPoint(), SketchLine_8.startPoint())
Sketch_3.setHorizontal(SketchLine_8.result())

SketchLine_9 = Sketch_3.addLine(0, 50, 100, 50)
Sketch_3.setCoincident(SketchLine_6.startPoint(), SketchLine_9.startPoint())
Sketch_3.setCoincident(SketchLine_5.endPoint(), SketchLine_9.endPoint())
Sketch_3.setHorizontal(SketchLine_9.result())
Sketch_3.setCoincident(SketchLine_5.startPoint(), SketchLine_8.startPoint())
Sketch_3.setCoincident(SketchLine_5.startPoint(), SketchLine_7.result())
Sketch_3.setCoincident(SketchLine_6.endPoint(), SketchLine_8.endPoint())
Sketch_3.setLength(SketchLine_9.result(), "largeur")
Sketch_3.setLength(SketchLine_8.result(), "largeur")

SketchProjection_5 = Sketch_3.addProjection(model.selection("VERTEX", "Sketch_2/SketchCircle_1_2__cc"), False)
SketchPoint_2 = SketchProjection_5.createdFeature()
Sketch_3.setVerticalDistance(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_9.startPoint(), "largeur/2")
Sketch_3.setVerticalDistance(SketchLine_5.startPoint(), SketchAPI_Point(SketchPoint_2).coordinates(), "largeur/2")
model.do()

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_3")], model.selection(), 100, 0, "Edges")

### Create Split
Split_1 = model.addSplit(Part_1_doc, [model.selection("SOLID", "ExtrusionCut_1_1")], [model.selection("COMPOUND", "Extrusion_2_1")], keepSubResults = True)

### Create FusionFaces
FusionFaces_1 = model.addFusionFaces(Part_1_doc, model.selection("COMPSOLID", "Split_1_1"))

model.end()

model.checkResult(FusionFaces_1, model, 1, [6], [6], [36], [144], [288])

assert(model.checkPythonDump())

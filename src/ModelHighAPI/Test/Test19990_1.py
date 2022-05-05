# Copyright (C) 2020-2022  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(56.24665913968287, -62.02452773486242, 35.57563613077872, -17.19150878248001, 88.70981046865168, -24.82997724138561, False)

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(88.70981046865168, -24.82997724138561, 35.57563613077872, -17.19150878248001)
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", (56.24665913968287, -64.29248088508524, 0))], model.selection(), 50, 0)

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.selection("FACE", (56.24665913968287, -64.29248088508524, 50)))

### Create SketchProjection
SketchProjection_1 = Sketch_2.addProjection(model.selection("VERTEX", (56.24665913968287, -62.02452773486242, 50)), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchArc
SketchArc_2 = Sketch_2.addArc(56.24665913968287, -62.02452773486242, 45.34418302532956, -36.70999577489869, 82.89389484228845, -54.98078276603709, False)
Sketch_2.setCoincident(SketchPoint_1.result(), SketchArc_2.center())

### Create SketchLine
SketchLine_2 = Sketch_2.addLine(82.89389484228843, -54.98078276603709, 45.34418302532956, -36.70999577489869)
Sketch_2.setCoincident(SketchArc_2.endPoint(), SketchLine_2.startPoint())
Sketch_2.setCoincident(SketchArc_2.startPoint(), SketchLine_2.endPoint())
model.do()

### Create ExtrusionCut
ExtrusionCut_1 = model.addExtrusionCut(Part_1_doc, [model.selection("WIRE", (64.11903893380899, -45.84538927046789, 50))], model.selection(), 0, 10, [model.selection("SOLID", (55.99758335516852, -63.75630399502394, 25))])

model.end()

from GeomAPI import *

model.testNbResults(ExtrusionCut_1, 1)
model.testNbSubResults(ExtrusionCut_1, [0])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.FACE, [7])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(ExtrusionCut_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(ExtrusionCut_1, [347314.47596])

assert(model.checkPythonDump())

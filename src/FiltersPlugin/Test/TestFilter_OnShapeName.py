# Copyright (C) 2014-2023  CEA, EDF
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

import sys
import salome

salome.salome_init()
import salome_notebook
notebook = salome_notebook.NoteBook()
sys.path.insert(0, r'/home/eksu/S2')

###
### SHAPER component
###

from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "rayon", '10.')
model.addParameter(Part_1_doc, "hauteur", '2.*rayon')

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_1 = SketchProjection_1.createdFeature()

### Create SketchCircle
SketchCircle_1 = Sketch_1.addCircle(0, 0, 10)
Sketch_1.setCoincident(SketchPoint_1.result(), SketchCircle_1.center())
Sketch_1.setRadius(SketchCircle_1.results()[1], "rayon ")
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), "hauteur ", "hauteur/4.", "Faces|Wires")

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_2.addLine(12.5, -12.5, -12.5, -12.5)

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_2 = SketchProjection_2.createdFeature()

### Create SketchLine
SketchLine_2 = Sketch_2.addLine(-12.5, -12.5, -12.5, 12.5)

### Create SketchLine
SketchLine_3 = Sketch_2.addLine(-12.5, 12.5, 12.5, 12.5)

### Create SketchLine
SketchLine_4 = Sketch_2.addLine(12.5, 12.5, 12.5, -12.5)
Sketch_2.setCoincident(SketchLine_4.endPoint(), SketchLine_1.startPoint())
Sketch_2.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_2.setCoincident(SketchLine_2.endPoint(), SketchLine_3.startPoint())
Sketch_2.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())

### Create SketchLine
SketchLine_5 = Sketch_2.addLine(12.5, -12.5, -12.5, 12.5)
SketchLine_5.setAuxiliary(True)

### Create SketchLine
SketchLine_6 = Sketch_2.addLine(-12.5, -12.5, 12.5, 12.5)
SketchLine_6.setAuxiliary(True)
Sketch_2.setCoincident(SketchLine_1.startPoint(), SketchLine_5.startPoint())
Sketch_2.setCoincident(SketchLine_2.startPoint(), SketchLine_6.startPoint())
Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchLine_5.endPoint())
Sketch_2.setCoincident(SketchLine_4.startPoint(), SketchLine_6.endPoint())
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_5.result())
Sketch_2.setCoincident(SketchAPI_Point(SketchPoint_2).coordinates(), SketchLine_6.result())
Sketch_2.setHorizontal(SketchLine_1.result())
Sketch_2.setVertical(SketchLine_2.result())
Sketch_2.setHorizontal(SketchLine_3.result())
Sketch_2.setVertical(SketchLine_4.result())
Sketch_2.setLength(SketchLine_3.result(), "2.5*rayon")
Sketch_2.setEqual(SketchLine_3.result(), SketchLine_2.result())
model.do()

### Create Extrusion
Extrusion_2 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_2")], model.selection(), 0, "2.*rayon", "Faces|Wires")

### Create Sketch
Sketch_3 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))

### Create SketchLine
SketchLine_7 = Sketch_3.addLine(-4.930380657631324e-32, -6, -4.930380657631324e-32, 6)

### Create SketchProjection
SketchProjection_3 = Sketch_3.addProjection(model.selection("VERTEX", "PartSet/Origin"), False)
SketchPoint_3 = SketchProjection_3.createdFeature()
Sketch_3.setMiddlePoint(SketchAPI_Point(SketchPoint_3).coordinates(), SketchLine_7.result())
Sketch_3.setVertical(SketchLine_7.result())
Sketch_3.setLength(SketchLine_7.result(), "1.2*rayon")
model.do()

### Create Extrusion
Extrusion_3 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_3")], model.selection("EDGE", "PartSet/OZ"), "hauteur*1.4", "hauteur*0.4", "Edges")

### Create Extrusion
Extrusion_4 = model.addExtrusion(Part_1_doc, [model.selection("VERTEX", "PartSet/Origin")], model.selection("EDGE", "PartSet/OY"), "rayon*1.6", "rayon*1.6")

### Create Translation
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("COMPOUND", "all-in-Extrusion_4")], axis = model.selection("EDGE", "PartSet/OZ"), distance = "hauteur*0.5", keepSubResults = True)

### Create Partition
Partition_1_objects = [model.selection("COMPOUND", "all-in-Extrusion_1"),
                       model.selection("COMPOUND", "all-in-Extrusion_2"),
                       model.selection("COMPOUND", "all-in-Extrusion_3"),
                       model.selection("COMPOUND", "all-in-Translation_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects, keepSubResults = True)
Partition_1.result().subResult(0).setName("Volumes")
Partition_1.result().subResult(0).subResult(0).setName("Cylindre bas")
Partition_1.result().subResult(0).subResult(1).setName("Cylindre haut")
Partition_1.result().subResult(0).subResult(2).setName("Cube Creux")
Partition_1.result().subResult(1).setName("Plaque")
Partition_1.result().subResult(2).setName("Poutre 1")
Partition_1.result().subResult(3).setName("Poutre 2")

### Create Group
Group_1_objects = [model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_4][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_3][Extrusion_2_1/From_Face]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_4][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_3][Cube Creux/Modified_Face&Extrusion_2_1/To_Face]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_4][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_1][Extrusion_2_1/From_Face]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_4][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_1][Cube Creux/Modified_Face&Extrusion_2_1/To_Face]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_3][Extrusion_2_1/From_Face][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_2]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_3][Cube Creux/Modified_Face&Extrusion_2_1/To_Face][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_2]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_1][Extrusion_2_1/From_Face][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_2]"),
                   model.selection("VERTEX", "[Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_1][Cube Creux/Modified_Face&Extrusion_2_1/To_Face][Extrusion_2_1/Generated_Face&Sketch_2/SketchLine_2]"),
                   model.selection("VERTEX", "[Extrusion_3_1/From_Edge]e[Cube Creux/Modified_Edge&Sketch_3/SketchLine_7_StartVertex]e"),
                   model.selection("VERTEX", "[Extrusion_3_1/From_Edge]e[Cube Creux/Modified_Edge&Sketch_3/SketchLine_7_EndVertex]e"), model.filters(Part_1_doc, [model.addFilter(name = "OnShapeName", args = ["Cube"])])]
Group_1 = model.addGroup(Part_1_doc, "Vertices", Group_1_objects)
Group_1.setName("CubeVertices_1")
Group_1.result().setName("CubeVertices_1")

### Create Group
Group_2_objects = [model.selection("EDGE", "(Cylindre haut/Generated_Edge&Extrusion_2_1/To_Face&Sketch_1/SketchCircle_1_2)([Cylindre haut/Modified_Face&Sketch_1/SketchCircle_1_2][Cylindre haut/Modified_Face&Extrusion_1_1/To_Face])"),
                   model.selection("EDGE", "[Cylindre haut/Modified_Face&Sketch_1/SketchCircle_1_2][Cylindre haut/Modified_Face&Extrusion_1_1/To_Face]"),
                   model.selection("EDGE", "Cylindre haut/Generated_Edge&Sketch_3/Sketch_3&Extrusion_1_1/To_Face"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&Sketch_3/SketchLine_7_EndVertex&new_weak_name_1"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&PartSet/Origin/Origin&new_weak_name_2"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&Sketch_3/SketchLine_7_StartVertex&new_weak_name_1"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&Sketch_3/SketchLine_7_EndVertex&new_weak_name_2"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&Sketch_3/SketchLine_7_StartVertex&new_weak_name_2"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&PartSet/Origin/Origin&new_weak_name_1"),
                   model.selection("EDGE", "Cylindre haut/Modified_Edge&PartSet/Origin/Origin&new_weak_name_3"),
                   model.selection("SOLID", "Cylindre bas"), model.filters(Part_1_doc, [model.addFilter(name = "OnShapeName", args = ["Cyl"])])]
Group_2 = model.addGroup(Part_1_doc, "Edges", Group_2_objects)
Group_2.setName("CylEdges_2")
Group_2.result().setName("CylEdges_2")

### Create Group
Group_3 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Plaque"), model.filters(Part_1_doc, [model.addFilter(name = "OnShapeName", args = ["Plaq"])])])
Group_3.setName("Plaque_3")
Group_3.result().setName("Plaque_3")

### Create Group
Group_4 = model.addGroup(Part_1_doc, "Solids", [model.selection("SOLID", "Cylindre haut"), model.selection("SOLID", "Cylindre bas"), model.filters(Part_1_doc, [model.addFilter(name = "OnShapeName", args = ["Cyl"])])])
Group_4.setName("Cyllindres_4")
Group_4.result().setName("Cyllindres_4")

model.end()

###
### SHAPERSTUDY component
###

model.publishToShaperStudy()
import SHAPERSTUDY
Partition_1_1, CubeVertices_1, CylEdges_2, Plaque_3, Cyllindres_4, = SHAPERSTUDY.shape(model.featureStringId(Partition_1))

if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()

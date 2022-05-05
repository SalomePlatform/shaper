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

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(35.66344827586208, 18.52827586206897, -10.58758620689655, 18.52827586206897)
Sketch_1.setHorizontal(SketchLine_1.result())

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(-10.58758620689655, 34.59034482758621, -10.58758620689655, 18.52827586206897, 0, 46.66896551724138, True)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchArc_1.startPoint())
Sketch_1.setTangent(SketchLine_1.result(), SketchArc_1.results()[1])
Sketch_1.setCoincident(SketchLine_2.result(), SketchArc_1.endPoint())
model.do()

### Create Sketch
Sketch_2 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))

### Create SketchLine
SketchLine_3 = Sketch_2.addLine(46.66896551724138, 0, 49.59252615395944, 57.3324122669822)

### Create SketchProjection
SketchProjection_2 = Sketch_2.addProjection(model.selection("VERTEX", "Sketch_1/SketchArc_1_2_StartVertex"), False)
SketchPoint_1 = SketchProjection_2.createdFeature()
Sketch_2.setCoincident(SketchLine_3.startPoint(), SketchPoint_1.result())
model.do()

### Create Wire
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchArc_1_2"), model.selection("EDGE", "Sketch_2/SketchLine_3")]
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)
model.end()


### Check errors on 1D-fillet
vertices = ["Sketch_1/SketchLine_1_StartVertex",
            "Sketch_2/SketchLine_3_EndVertex",
            "[Wire_1_1/Modified_Edge&Sketch_1/SketchLine_1]e[Wire_1_1/Modified_Edge&Sketch_1/SketchArc_1_2]e",
            "[Wire_1_1/Modified_Edge&Sketch_1/SketchArc_1_2]e[Wire_1_1/Modified_Edge&Sketch_2/SketchLine_3]e"]
for v in vertices:
  model.begin()
  Fillet1D = model.addFillet(Part_1_doc, [model.selection("VERTEX", v)], 1)
  model.end()
  assert(Fillet1D.feature().error() != "")

### Wire has no vertices applicable for 1D-fillet
model.begin()
Fillet1D = model.addFillet(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], 1)
model.end()
assert(Fillet1D.feature().error() != "")

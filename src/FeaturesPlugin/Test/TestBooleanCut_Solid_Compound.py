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

###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "nb", '6')

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.selection("FACE", "Cylinder_1_1/Face_3"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(-3.635759493440736, -2.855949413069177, 3.547995857301719, -2.855949413069177)

### Create SketchProjection
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_2 = SketchProjection_1.createdFeature()

### Create SketchLine
SketchLine_3 = Sketch_1.addLine(3.547995857301719, -2.855949413069177, 3.547995857301719, -5.405023892364889)

### Create SketchLine
SketchLine_4 = Sketch_1.addLine(3.547995857301719, -5.405023892364889, -3.635759493440736, -5.405023892364889)

### Create SketchLine
SketchLine_5 = Sketch_1.addLine(-3.635759493440736, -5.405023892364889, -3.635759493440736, -2.855949413069177)
Sketch_1.setCoincident(SketchLine_5.endPoint(), SketchLine_1.startPoint())
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_3.startPoint())
Sketch_1.setCoincident(SketchLine_3.endPoint(), SketchLine_4.startPoint())
Sketch_1.setCoincident(SketchLine_4.endPoint(), SketchLine_5.startPoint())

### Create SketchLine
SketchLine_6 = Sketch_1.addLine(-3.635759493440736, -2.855949413069177, 3.547995857301719, -5.405023892364889)
SketchLine_6.setAuxiliary(True)

### Create SketchLine
SketchLine_7 = Sketch_1.addLine(3.547995857301719, -2.855949413069177, -3.635759493440736, -5.405023892364889)
SketchLine_7.setAuxiliary(True)
Sketch_1.setCoincident(SketchLine_1.startPoint(), SketchLine_6.startPoint())
Sketch_1.setCoincident(SketchLine_3.startPoint(), SketchLine_7.startPoint())
Sketch_1.setCoincident(SketchLine_4.startPoint(), SketchLine_6.endPoint())
Sketch_1.setCoincident(SketchLine_5.startPoint(), SketchLine_7.endPoint())
Sketch_1.setHorizontal(SketchLine_1.result())
Sketch_1.setVertical(SketchLine_3.result())
Sketch_1.setHorizontal(SketchLine_4.result())
Sketch_1.setVertical(SketchLine_5.result())
model.do()

### Create Extrusion
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("FACE", "Sketch_1/Face-SketchLine_5r-SketchLine_4r-SketchLine_3r-SketchLine_1r")], model.selection(), 1, 11, "Faces|Wires")

### Create AngularCopy
AngularCopy_1 = model.addMultiRotation(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1")], model.selection("EDGE", "PartSet/OZ"), "nb", keepSubResults = True)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], [model.selection("COMPOUND", "AngularCopy_1_1")], keepSubResults = True)

model.do()
model.end()

model.testResults(Cut_1, 1, [0], [1], [8], [18], [12])
model.testResultsVolumes(Cut_1, [282.54761399726])

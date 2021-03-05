# Copyright (C) 2021  CEA/DEN, EDF R&D
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
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("YOZ"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(-62.43672548686145, -3.439140342201228, 39.05146750980462, 70.99404207935532)
SketchLine_1.setAuxiliary(True)

### Create SketchEllipse
SketchEllipse_1 = Sketch_1.addEllipse(-11.02958888389993, 34.2637358599971, -5.505025597864623, 77.21113625278682, 25)
[SketchPoint_1, SketchPoint_2, SketchPoint_3, SketchPoint_4, SketchPoint_5, SketchPoint_6, SketchPoint_7, SketchLine_2, SketchLine_3] = SketchEllipse_1.construction(center = "aux", firstFocus = "aux", secondFocus = "aux", majorAxisStart = "aux", majorAxisEnd = "aux", minorAxisStart = "aux", minorAxisEnd = "aux", majorAxis = "aux", minorAxis = "aux")
Sketch_1.setCoincident(SketchLine_1.result(), SketchEllipse_1.center())
Sketch_1.setLength(SketchLine_3.result(), 50)
Sketch_1.setLength(SketchLine_2.result(), 100)

### Create SketchConstraintAngle
Sketch_1.setAngle(SketchLine_1.result(), SketchLine_2.result(), 0, type = "Direct")
model.do()

model.end()

assert(Sketch_1.feature().error() == "")

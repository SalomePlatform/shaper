# Copyright (C) 2020  CEA/DEN, EDF R&D
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

### Create Box
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_1.setName("boîte")
Box_1.result().setName("boîte")

### Create Sphere
Sphere_1 = model.addSphere(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), 10)
Sphere_1.setName("sphère")
Sphere_1.result().setName("sphère")

### Create Shell
Shell_1_objects = [model.selection("FACE", "boîte/Left"),
                   model.selection("FACE", "boîte/Back"),
                   model.selection("FACE", "boîte/Bottom")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)

### Create Cut
Cut_1 = model.addCut(Part_1_doc, [model.selection("SHELL", "boîte")], [model.selection("COMPOUND", "all-in-sphère")], keepSubResults = True)
Cut_1.result().subResult(0).setName("Cut_1_1_1")
Cut_1.result().subResult(1).setName("Cut_1_1_2")
Cut_1.result().subResult(2).setName("Cut_1_1_3")

model.end()

from GeomAPI import *

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [9])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [18])
model.testResultsVolumes(Cut_1, [0.0])
model.testResultsAreas(Cut_1, [64.38055098])

assert(model.checkPythonDump())

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
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Sketch
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))

### Create SketchLine
SketchLine_1 = Sketch_1.addLine(-21.09850585253612, 7.838733928946021, 28.18352301808598, -36.47460859551143)

### Create SketchLine
SketchLine_2 = Sketch_1.addLine(28.18352301808598, -36.47460859551143, -24.86780397022334, -36.47460859551143)
Sketch_1.setCoincident(SketchLine_1.endPoint(), SketchLine_2.startPoint())
Sketch_1.setHorizontal(SketchLine_2.result())

### Create SketchArc
SketchArc_1 = Sketch_1.addArc(-24.86780397022334, -14.15762886307242, -24.86780397022334, -36.47460859551143, -21.09850585253612, 7.838733928946021, True)
Sketch_1.setCoincident(SketchLine_2.endPoint(), SketchArc_1.startPoint())
Sketch_1.setTangent(SketchLine_2.result(), SketchArc_1.results()[1])
Sketch_1.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
model.do()
Wire_1_objects = [model.selection("EDGE", "Sketch_1/SketchLine_1"), model.selection("EDGE", "Sketch_1/SketchLine_2"), model.selection("EDGE", "Sketch_1/SketchArc_1_2")]

### Create Wire
Wire_1 = model.addWire(Part_1_doc, Wire_1_objects, False)

### Create Fillet1D
Fillet1D_1 = model.addFillet(Part_1_doc, [model.selection("WIRE", "Wire_1_1")], 2)

model.testHaveNamingByType(Fillet1D_1, model, Part_1_doc, GeomAPI_Shape.VERTEX)
model.testHaveNamingByType(Fillet1D_1, model, Part_1_doc, GeomAPI_Shape.EDGE)
model.end()

model.testNbResults(Fillet1D_1, 1)
model.testNbSubResults(Fillet1D_1, [0])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.EDGE, [5])
model.testNbSubShapes(Fillet1D_1, GeomAPI_Shape.VERTEX, [10])
model.testResultsVolumes(Fillet1D_1, [0])

assert(model.checkPythonDump(model.ModelHighAPI.CHECK_NAMING))

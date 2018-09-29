## Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_BoxH = model.addParameter(Part_1_doc, "BoxHeight", "10")
Param_ExtrusionMin = model.addParameter(Part_1_doc, "ExtrusionMin", "0")
Param_ExtrusionMax = model.addParameter(Part_1_doc, "ExtrusionMax", "10")
Param_Radius = model.addParameter(Part_1_doc, "Radius", "40")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(47, -3, 25)
SketchCircle_2 = Sketch_1.addCircle(55, 30, 40)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_2.results()[1], "Radius")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), "ExtrusionMax", "ExtrusionMin")
Box_1 = model.addBox(Part_1_doc, 50, 100, "BoxHeight")
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Extrusion_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2")])
model.do()

# check partition
model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [6])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [6])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [39])
model.testResultsVolumes(Partition_1, [88456.153365924168610945343971252])

# change box height and check partition
Param_BoxH.setValue(20)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [6])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [6])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [41])
model.testResultsVolumes(Partition_1, [138456.153365848527755588293075562])

# update extrusion and check partition
Param_ExtrusionMin.setValue(-5)
Param_ExtrusionMax.setValue(15)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [6])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [6])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [43])
model.testResultsVolumes(Partition_1, [138456.153365848527755588293075562])

# change radius of a circle and check partition
Param_Radius.setValue(50)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [7])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [7])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [46])
model.testResultsVolumes(Partition_1, [151872.052359062305185943841934204])

# check naming
model.testHaveNamingSubshapes(Partition_1, model, Part_1_doc)

model.end()

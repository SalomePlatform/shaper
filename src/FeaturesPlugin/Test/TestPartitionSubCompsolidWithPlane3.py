# Copyright (C) 2014-2024  CEA, EDF
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
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Param_ExtrusionMin = model.addParameter(Part_1_doc, "ExtrusionMin", "0")
Param_ExtrusionMax = model.addParameter(Part_1_doc, "ExtrusionMax", "10")
Param_Radius = model.addParameter(Part_1_doc, "Radius", "40")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchCircle_1 = Sketch_1.addCircle(47, -3, 25)
SketchCircle_2 = Sketch_1.addCircle(55, 30, 40)
SketchConstraintRadius_1 = Sketch_1.setRadius(SketchCircle_2.results()[1], "Radius")
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "Sketch_1")], model.selection(), "ExtrusionMax", "ExtrusionMin")
Partition_1 = model.addPartition(Part_1_doc, [model.selection("SOLID", "Extrusion_1_1_3"), model.selection("FACE", "PartSet/XOZ")])
model.do()

# check partition
model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [22])
model.testResultsVolumes(Partition_1, [58449.978432346062618307769298553])

# change radius of a circle and check partition
Param_Radius.setValue(50)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [3])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [15])
model.testResultsVolumes(Partition_1, [81554.943249273565015755593776703])

# update extrusion and check partition
Param_Radius.setValue(40)
Param_ExtrusionMin.setValue(-5)
Param_ExtrusionMax.setValue(15)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [22])
model.testResultsVolumes(Partition_1, [58449.978432346062618307769298553])

# check naming
model.testHaveNamingSubshapes(Partition_1, model, Part_1_doc)

model.end()

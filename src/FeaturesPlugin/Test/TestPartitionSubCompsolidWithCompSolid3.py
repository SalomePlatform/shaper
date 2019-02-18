# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

# -*- coding: utf-8 -*-

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
Box_1 = model.addBox(Part_1_doc, 80, 80, "BoxHeight")
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Left"), model.selection("FACE", "Box_1_1/Right"))
Plane_5 = model.addPlane(Part_1_doc, model.selection("FACE", "Box_1_1/Back"), model.selection("FACE", "Box_1_1/Front"))
Partition_1_objects = [model.selection("SOLID", "Box_1_1"), model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
Partition_2_objects = [model.selection("SOLID", "Extrusion_1_1_3"), model.selection("SOLID", "Partition_1_1_1"), model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Partition_1_1_3")]
Partition_2 = model.addPartition(Part_1_doc, Partition_2_objects)
model.do()

# check partition
model.testNbResults(Partition_2, 1)
model.testNbSubResults(Partition_2, [9])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.SOLID, [9])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.FACE, [63])
model.testResultsVolumes(Partition_2, [82157.197962205173098482191562653])

# change box height and check partition
Param_BoxH.setValue(20)
model.do()

model.testNbResults(Partition_2, 1)
model.testNbSubResults(Partition_2, [10])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.SOLID, [10])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.FACE, [81])
model.testResultsVolumes(Partition_2, [146157.197962133242981508374214172])

# update extrusion and check partition
Param_ExtrusionMin.setValue(-5)
Param_ExtrusionMax.setValue(15)
model.do()

model.testNbResults(Partition_2, 1)
model.testNbSubResults(Partition_2, [11])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.SOLID, [11])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.FACE, [99])
model.testResultsVolumes(Partition_2, [146157.197962085192557424306869507])

# change radius of a circle and check partition
Param_Radius.setValue(50)
model.do()

model.testNbResults(Partition_2, 1)
model.testNbSubResults(Partition_2, [11])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.SOLID, [11])
model.testNbSubShapes(Partition_2, GeomAPI_Shape.FACE, [98])
model.testResultsVolumes(Partition_2, [156418.581829176022438332438468933])

# check naming
model.testHaveNamingSubshapes(Partition_2, model, Part_1_doc)

model.end()

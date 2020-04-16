# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
Sketch_2 = model.addSketch(Part_1_doc, model.standardPlane("XOY"))
SketchArc_1 = Sketch_2.addArc(10.01336797518561, 24.95055816041118, 7.898114670281987, -23.85096600991564, 58.22303946797933, 32.81761798074749, False)
SketchLine_1 = Sketch_2.addLine(58.22303949051827, 32.8176179844255, 7.898114670281987, -23.85096600991564)
SketchConstraintCoincidence_1 = Sketch_2.setCoincident(SketchArc_1.endPoint(), SketchLine_1.startPoint())
SketchConstraintCoincidence_2 = Sketch_2.setCoincident(SketchArc_1.startPoint(), SketchLine_1.endPoint())
model.do()
Revolution_1 = model.addRevolution(Part_1_doc, [model.selection("COMPOUND", "Sketch_2")], model.selection("EDGE", "Sketch_2/SketchLine_1"), 90, 0)
Face_1 = model.addFace(Part_1_doc, [model.selection("FACE", "Revolution_1_1/Generated_Face&Sketch_2/SketchArc_1_2")])
Partition_1_objects = [model.selection("SOLID", "Extrusion_1_1_2"), model.selection("SOLID", "Extrusion_1_1_3"), model.selection("FACE", "Face_1_1")]
Partition_1 = model.addPartition(Part_1_doc, Partition_1_objects)
model.do()

# check partition
model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [27])
model.testResultsVolumes(Partition_1, [58449.978429485141532495617866516])

# update extrusion and check partition
Param_ExtrusionMin.setValue(-5)
Param_ExtrusionMax.setValue(15)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [25])
model.testResultsVolumes(Partition_1, [58449.978433180862339213490486145])

# change radius of a circle and check partition
Param_Radius.setValue(50)
model.do()

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [2])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.FACE, [25])
model.testResultsVolumes(Partition_1, [81554.943249094052589498460292816])

# check naming
model.testHaveNamingSubshapes(Partition_1, model, Part_1_doc)

model.end()

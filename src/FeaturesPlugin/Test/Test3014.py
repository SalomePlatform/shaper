# Copyright (C) 2019  CEA/DEN, EDF R&D
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

from GeomAPI import *
from SketchAPI import *

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
model.addParameter(Part_1_doc, "nb", "10")
model.addParameter(Part_1_doc, "h", "20")
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOY"))
SketchProjection_1 = Sketch_1.addProjection(model.selection("EDGE", "PartSet/OY"), False)
SketchLine_1 = SketchProjection_1.createdFeature()
SketchCircle_1 = Sketch_1.addCircle(0, 55.86212361331221, 9.333219356250011)
SketchConstraintCoincidence_1 = Sketch_1.setCoincident(SketchLine_1.result(), SketchCircle_1.center())
SketchMultiRotation_1 = Sketch_1.addRotation([SketchCircle_1.results()[1]], SketchAPI_Line(SketchLine_1).startPoint(), 360, "nb", True)
[SketchCircle_2, SketchCircle_3, SketchCircle_4, SketchCircle_5, SketchCircle_6, SketchCircle_7, SketchCircle_8, SketchCircle_9, SketchCircle_10] = SketchMultiRotation_1.rotated()
model.do()
Extrusion_1 = model.addExtrusion(Part_1_doc, [model.selection("COMPOUND", "all-in-Sketch_1")], model.selection(), "h", 0)
Plane_4 = model.addPlane(Part_1_doc, model.selection("FACE", "Extrusion_1_4/To_Face"), "h/2", True)
Partition_1 = model.addPartition(Part_1_doc, [model.selection("FACE", "Plane_1"), model.selection("COMPOUND", "all-in-Extrusion_1")], keepSubResults = True)
model.end()

assert(Partition_1.feature().error() == "")

model.testNbResults(Partition_1, 1)
model.testNbSubResults(Partition_1, [10])
model.testNbSubShapes(Partition_1, GeomAPI_Shape.SOLID, [20])

assert(model.checkPythonDump())

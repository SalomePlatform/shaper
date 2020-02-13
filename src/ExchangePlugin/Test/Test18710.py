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
from ModelAPI import *
from SketchAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Export_1 = model.exportToXAO(Part_1_doc, '/tmp/shaper_vcnhioqf.xao', model.selection("SOLID", "Cylinder_1_1"), 'XAO')
Export_1.setName("XAO")
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
model.end()

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [9])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [30])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [60])
model.testResultsVolumes(Part_1, [1785.39816339744857])

model.begin()
ModelAPI.removeFeaturesAndReferences(FeatureSet([Cylinder_1.feature()]))
model.end()

model.testNbResults(Part_1, 1)
model.testNbSubResults(Part_1, [0])
model.testNbSubShapes(Part_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Part_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Part_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Part_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Part_1, [1000])

assert(model.checkPythonDump())

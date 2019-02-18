# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
ConeSegment_1 = model.addConeSegment(Part_1_doc, 7, 11, 5, 8, 12, 0, 270)
model.end()

model.testNbResults(ConeSegment_1, 1)
model.testNbSubResults(ConeSegment_1, [0])
model.testNbSubShapes(ConeSegment_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(ConeSegment_1, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(ConeSegment_1, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(ConeSegment_1, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(ConeSegment_1, [1545.663585663386584201361984015])

assert(model.checkPythonDump())

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
from GeomAPI import GeomAPI_Shape

model.begin()
partSet = model.moduleDocument()

Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Ellipsoid_1 = model.addEllipsoid(Part_1_doc, 10, 20, 40)

model.testNbResults(Ellipsoid_1, 1)
model.testNbSubResults(Ellipsoid_1, [0])
model.testNbSubShapes(Ellipsoid_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Ellipsoid_1, GeomAPI_Shape.FACE, [1])
model.testNbSubShapes(Ellipsoid_1, GeomAPI_Shape.EDGE, [4])
model.testNbSubShapes(Ellipsoid_1, GeomAPI_Shape.VERTEX, [8])
model.testResultsVolumes(Ellipsoid_1, [4188.80255909031])

Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Ellipsoid_2 = model.addEllipsoid(Part_2_doc, 20, 10, 30, 5, 5)

model.testNbResults(Ellipsoid_2, 1)
model.testNbSubResults(Ellipsoid_2, [0])
model.testNbSubShapes(Ellipsoid_2, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Ellipsoid_2, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Ellipsoid_2, GeomAPI_Shape.EDGE, [6])
model.testNbSubShapes(Ellipsoid_2, GeomAPI_Shape.VERTEX, [12])
model.testResultsVolumes(Ellipsoid_2, [1512.62177363772])

Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Ellipsoid_3 = model.addEllipsoid(Part_3_doc, 20, 20, 40)
Ellipsoid_3.setZCut1(5)

model.testNbResults(Ellipsoid_3, 1)
model.testNbSubResults(Ellipsoid_3, [0])
model.testNbSubShapes(Ellipsoid_3, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Ellipsoid_3, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Ellipsoid_3, GeomAPI_Shape.EDGE, [7])
model.testNbSubShapes(Ellipsoid_3, GeomAPI_Shape.VERTEX, [14])
model.testResultsVolumes(Ellipsoid_3, [5726.87643051263])

Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()
Ellipsoid_4 = model.addEllipsoid(Part_4_doc, 20, 20, 40)
Ellipsoid_4.setZCut2(5)

model.testNbResults(Ellipsoid_4, 1)
model.testNbSubResults(Ellipsoid_4, [0])
model.testNbSubShapes(Ellipsoid_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Ellipsoid_4, GeomAPI_Shape.FACE, [2])
model.testNbSubShapes(Ellipsoid_4, GeomAPI_Shape.EDGE, [7])
model.testNbSubShapes(Ellipsoid_4, GeomAPI_Shape.VERTEX, [14])
model.testResultsVolumes(Ellipsoid_4, [5726.87643051263])

# Check ellipsoid errors
Part_5 = model.addPart(partSet)
Part_5_doc = Part_5.document()
Ellipsoid_5 = model.addEllipsoid(Part_5_doc, 0, 20, 30)
model.do()
assert(Ellipsoid_5.feature().error() != "")
Part_5_doc.removeFeature(Ellipsoid_5.feature())

Ellipsoid_5 = model.addEllipsoid(Part_5_doc, 10, 0, 30)
model.do()
assert(Ellipsoid_5.feature().error() != "")
Part_5_doc.removeFeature(Ellipsoid_5.feature())

Ellipsoid_5 = model.addEllipsoid(Part_5_doc, 10, 20, 0)
model.do()
assert(Ellipsoid_5.feature().error() != "")
Part_5_doc.removeFeature(Ellipsoid_5.feature())

Ellipsoid_5 = model.addEllipsoid(Part_5_doc, 10, 20, 30, -5, 5)
model.do()
assert(Ellipsoid_5.feature().error() != "")
Part_5_doc.removeFeature(Ellipsoid_5.feature())

Ellipsoid_5 = model.addEllipsoid(Part_5_doc, 10, 20, 30, 5, -5)
model.do()
assert(Ellipsoid_5.feature().error() != "")
Part_5_doc.removeFeature(Ellipsoid_5.feature())

Ellipsoid_5 = model.addEllipsoid(Part_5_doc, 10, 20, 30, 0, 0)
model.do()
assert(Ellipsoid_5.feature().error() != "")
Part_5_doc.removeFeature(Ellipsoid_5.feature())

partSet.removeFeature(Part_5.feature())

model.end()

assert(model.checkPythonDump())

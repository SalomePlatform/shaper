# Copyright (C) 2018-2022  CEA/DEN, EDF R&D
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
Point_2 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 0, 10, 0)
Point_4 = model.addPoint(Part_1_doc, 20, 10, 0)
Point_5 = model.addPoint(Part_1_doc, 10, 20, 0)
Polyline_1 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_4")], False)
Polyline_2 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_2"), model.selection("VERTEX", "Point_3")], False)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("WIRE", "Polyline_1_1"), model.selection("WIRE", "Polyline_2_1")])
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("EDGE", "Compound_1_1_2", 1)], model.selection("EDGE", "PartSet/OY"), 10)
model.do()

model.testNbResults(Translation_1, 1)
model.testNbSubResults(Translation_1, [0])
model.testNbSubShapes(Translation_1, GeomAPI_Shape.SOLID, [0])
model.testNbSubShapes(Translation_1, GeomAPI_Shape.FACE, [0])
model.testNbSubShapes(Translation_1, GeomAPI_Shape.EDGE, [1])
model.testNbSubShapes(Translation_1, GeomAPI_Shape.VERTEX, [2])


Part_2 = model.addPart(partSet)
Part_2_doc = Part_2.document()
Box_1 = model.addBox(Part_2_doc, 10, 10, 10)
Box_2 = model.addBox(Part_2_doc, 4, 4, 4)
Translation_2 = model.addTranslation(Part_2_doc, [model.selection("SOLID", "Box_2_1")], 3, 3, 3)
Compound_2 = model.addCompound(Part_2_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Translation_1_1")])
Translation_3 = model.addTranslation(Part_2_doc, [model.selection("SOLID", "Compound_1_1", 0)], 10, 0, 0)
model.do()

model.testNbResults(Translation_3, 1)
model.testNbSubResults(Translation_3, [2])
model.testNbSubShapes(Translation_3, GeomAPI_Shape.SOLID, [2])
model.testNbSubShapes(Translation_3, GeomAPI_Shape.FACE, [12])
model.testNbSubShapes(Translation_3, GeomAPI_Shape.EDGE, [48])
model.testNbSubShapes(Translation_3, GeomAPI_Shape.VERTEX, [96])
model.testResultsVolumes(Translation_3, [1064])


Part_3 = model.addPart(partSet)
Part_3_doc = Part_3.document()
Box_3 = model.addBox(Part_3_doc, 10, 10, 10)
Box_4 = model.addBox(Part_3_doc, 10, 10, 10)
Point_6 = model.addPoint(Part_3_doc, 5, 5, 0)
Point_7 = model.addPoint(Part_3_doc, 5, 5, 10)
Axis_4 = model.addAxis(Part_3_doc, model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2"))
Rotation_1 = model.addRotation(Part_3_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "Axis_1"), 45)
Compound_3 = model.addCompound(Part_3_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Rotation_1_1")])
Translation_4 = model.addTranslation(Part_3_doc, [model.selection("SOLID", "Compound_1_1", 1)], 10, 0, 0)
model.do()

model.testNbResults(Translation_4, 1)
model.testNbSubResults(Translation_4, [0])
model.testNbSubShapes(Translation_4, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_4, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Translation_4, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Translation_4, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Translation_4, [1000])


Part_4 = model.addPart(partSet)
Part_4_doc = Part_4.document()
Box_5 = model.addBox(Part_4_doc, 10, 10, 10)
Box_6 = model.addBox(Part_4_doc, 10, 10, 10)
Compound_4 = model.addCompound(Part_4_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Box_2_1")])
Translation_5 = model.addTranslation(Part_4_doc, [model.selection("SOLID", "Compound_1_1", 1)], model.selection("EDGE", "PartSet/OZ"), 10)
model.do()

model.testNbResults(Translation_5, 1)
model.testNbSubResults(Translation_5, [0])
model.testNbSubShapes(Translation_5, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Translation_5, GeomAPI_Shape.FACE, [6])
model.testNbSubShapes(Translation_5, GeomAPI_Shape.EDGE, [24])
model.testNbSubShapes(Translation_5, GeomAPI_Shape.VERTEX, [48])
model.testResultsVolumes(Translation_5, [1000])

model.end()

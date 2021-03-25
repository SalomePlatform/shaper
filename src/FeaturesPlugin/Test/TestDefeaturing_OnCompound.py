# Copyright (C) 2020-2021  CEA/DEN, EDF R&D
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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 20, 20, 10)
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cylinder_1_1")], 10, 10, 0)
Cylinder_2 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Box_1_1")], [model.selection("SOLID", "Translation_1_1"), model.selection("SOLID", "Cylinder_2_1")], keepSubResults = True)
LinearCopy_1 = model.addMultiTranslation(Part_1_doc, [model.selection("SOLID", "Cut_1_1")], model.selection("EDGE", "PartSet/OX"), 30, 2, model.selection("EDGE", "PartSet/OY"), 30, 2)
Defeaturing_1_objects = [model.selection("FACE", "LinearCopy_1_1_1/MF:Translated&Cylinder_2_1/Face_1"), model.selection("FACE", "LinearCopy_1_1_4/MF:Translated&Cylinder_1_1/Face_1"), model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Cylinder_2_1/Face_1"), model.selection("FACE", "LinearCopy_1_1_3/MF:Translated&Cylinder_1_1/Face_1")]
Defeaturing_1 = model.addDefeaturing(Part_1_doc, Defeaturing_1_objects)
model.testHaveNamingSubshapes(Defeaturing_1, model, Part_1_doc)
model.end()

from GeomAPI import *

model.testNbResults(Defeaturing_1, 1)
model.testNbSubResults(Defeaturing_1, [4])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.SOLID, [4])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.FACE, [28])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.EDGE, [120])
model.testNbSubShapes(Defeaturing_1, GeomAPI_Shape.VERTEX, [240])
model.testResultsVolumes(Defeaturing_1, [14036.50459153])

assert(model.checkPythonDump())

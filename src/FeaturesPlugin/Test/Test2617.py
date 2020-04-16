# Copyright (C) 2018-2020  CEA/DEN, EDF R&D
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
Cone_1 = model.addCone(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 10, 5, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Cone_1_1")], model.selection("EDGE", "PartSet/OX"), 5)
Symmetry_1 = model.addSymmetry(Part_1_doc, [model.selection("SOLID", "Translation_1_1")], model.selection("EDGE", "PartSet/OZ"), True)
Common_1 = model.addCommon(Part_1_doc, [model.selection("SOLID", "Symmetry_1_1_2"), model.selection("SOLID", "Symmetry_1_1_1")])
model.do()
model.testHaveNamingSubshapes(Common_1, model, Part_1_doc)
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Common_1, 1)
model.testNbSubResults(Common_1, [0])
model.testNbSubShapes(Common_1, GeomAPI_Shape.SOLID, [1])
model.testNbSubShapes(Common_1, GeomAPI_Shape.FACE, [3])
model.testNbSubShapes(Common_1, GeomAPI_Shape.EDGE, [8])
model.testNbSubShapes(Common_1, GeomAPI_Shape.VERTEX, [16])
model.testResultsVolumes(Common_1, [461.055845803990962394891539589])

assert(model.checkPythonDump())

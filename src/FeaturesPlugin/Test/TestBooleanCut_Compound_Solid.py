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

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Box_2 = model.addBox(Part_1_doc, 10, 10, 10)
Translation_1 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_2_1")], model.selection("EDGE", "PartSet/OX"), 20)
Compound_1 = model.addCompound(Part_1_doc, [model.selection("SOLID", "Box_1_1"), model.selection("SOLID", "Translation_1_1")])
Box_3 = model.addBox(Part_1_doc, 20, 6, 20)
Translation_2 = model.addTranslation(Part_1_doc, [model.selection("SOLID", "Box_3_1")], -5, 2, -5)
Cut_1 = model.addCut(Part_1_doc, [model.selection("SOLID", "Compound_1_1_1")], [model.selection("SOLID", "Translation_2_1")])
model.do()
model.end()

from GeomAPI import GeomAPI_Shape

model.testNbResults(Cut_1, 1)
model.testNbSubResults(Cut_1, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.SOLID, [3])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.FACE, [18])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.EDGE, [72])
model.testNbSubShapes(Cut_1, GeomAPI_Shape.VERTEX, [144])
model.testResultsVolumes(Cut_1, [1400])

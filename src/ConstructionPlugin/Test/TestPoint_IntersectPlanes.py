## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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

"""
Test case for Construction Point feature by planes intersection.
"""

from salome.shaper import model
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Point_2 = model.addPoint(Part_1_doc, 50, 50, 50)
Plane_4 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OY"), model.selection("VERTEX", "Point_1"), False)
Plane_5 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OX"), model.selection("VERTEX", "Point_1"), False)
Plane_6 = model.addPlane(Part_1_doc, model.selection("EDGE", "PartSet/OZ"), model.selection("VERTEX", "Point_1"), True)
Point_3 = model.addPoint(Part_1_doc, model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_2"), model.selection("FACE", "Plane_3"))
Box_1 = model.addBox(Part_1_doc, 50, 25, 100)
Point_4 = model.addPoint(Part_1_doc, model.selection("FACE", "Plane_1"), model.selection("FACE", "Plane_3"), model.selection("FACE", "Box_1_1/Right"))
model.do()
model.end()

assert (len(Point_3.results()) > 0)
rightPosition = GeomAPI_Vertex(50, 50, 50)
assert(rightPosition.isEqual(Point_3.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_4.results()) > 0)
rightPosition = GeomAPI_Vertex(50, 25, 50)
assert(rightPosition.isEqual(Point_4.results()[0].resultSubShapePair()[0].shape()))

assert(model.checkPythonDump())

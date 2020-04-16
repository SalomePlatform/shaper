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

"""
Test case for Construction Point feature by projection point on face.
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchPoint_1 = Sketch_1.addPoint(50, 50)
model.do()
Box_1 = model.addBox(Part_1_doc, 10, 100, 100)
Point_2 = model.addPoint(Part_1_doc, model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("FACE", "Box_1_1/Front"))
Point_3 = model.addPoint(Part_1_doc, model.selection("VERTEX", "Sketch_1/SketchPoint_1"), model.selection("FACE", "PartSet/XOY"))
model.do()
model.end()

from GeomAPI import GeomAPI_Vertex

assert (len(Point_2.results()) > 0)
rightPosition = GeomAPI_Vertex(10, 0, 50)
assert(rightPosition.isEqual(Point_2.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_3.results()) > 0)
rightPosition = GeomAPI_Vertex(50, 0, 0)
assert(rightPosition.isEqual(Point_3.results()[0].resultSubShapePair()[0].shape()))

assert(model.checkPythonDump())

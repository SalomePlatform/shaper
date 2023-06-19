# Copyright (C) 2014-2023  CEA, EDF
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
Test case for Construction Point feature by center of gravity.
"""

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Sketch_1 = model.addSketch(Part_1_doc, model.defaultPlane("XOZ"))
SketchCircle_1 = Sketch_1.addCircle(-50, 25, 25)
SketchCircle_2 = Sketch_1.addCircle(50, 25, 25)
model.do()
Box_1 = model.addBox(Part_1_doc, 50, 50, 50)
Point_2 = model.addPoint(Part_1_doc, model.selection("COMPOUND", "Sketch_1"))
Point_3 = model.addPoint(Part_1_doc, model.selection("SOLID", "Box_1_1"))
Point_4 = model.addPoint(Part_1_doc, model.selection("FACE", "Box_1_1/Top"))
Point_5 = model.addPoint(Part_1_doc, model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"))
model.do()
model.end()

from GeomAPI import GeomAPI_Vertex

assert (len(Point_2.results()) > 0)
rightPosition = GeomAPI_Vertex(0, 0, 25)
assert(rightPosition.isEqual(Point_2.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_3.results()) > 0)
rightPosition = GeomAPI_Vertex(25, 25, 25)
assert(rightPosition.isEqual(Point_3.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_4.results()) > 0)
rightPosition = GeomAPI_Vertex(25, 25, 50)
assert(rightPosition.isEqual(Point_4.results()[0].resultSubShapePair()[0].shape()))

assert (len(Point_5.results()) > 0)
rightPosition = GeomAPI_Vertex(25, 0, 50)
assert(rightPosition.isEqual(Point_5.results()[0].resultSubShapePair()[0].shape()))

assert(model.checkPythonDump())

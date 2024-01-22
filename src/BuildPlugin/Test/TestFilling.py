# Copyright (C) 2017-2024  CEA, EDF
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
Point_2 = model.addPoint(Part_1_doc, 10, 0, 0)
Point_3 = model.addPoint(Part_1_doc, 20, 10, 0)
Point_4 = model.addPoint(Part_1_doc, 0, 20, 10)
Point_5 = model.addPoint(Part_1_doc, 30, 0, 10)
Polyline_1 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_1"), model.selection("VERTEX", "Point_2")], False)
Polyline_2 = model.addPolyline3D(Part_1_doc, [model.selection("VERTEX", "Point_3"), model.selection("VERTEX", "Point_4")], False)
Filling_1 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1"), model.selection("EDGE", "Polyline_2_1/Generated_Edge&Point_3/Point_3")])
Filling_2 = model.addFilling(Part_1_doc, [model.selection("EDGE", "Polyline_1_1/Generated_Edge&Point_1/Point_1"), model.selection("EDGE", "Polyline_2_1/Generated_Edge&Point_3/Point_3")], "curve_info", 5, 8, 0, 0.0001, 0.0001, True)
model.do()

assert(Filling_1.feature().error() == "")
assert(Filling_2.feature().error() == "")

model.end()

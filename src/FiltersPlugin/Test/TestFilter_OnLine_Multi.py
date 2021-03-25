# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
# two filters with two edges each
Filters = model.filters(Part_1_doc, [
  model.addFilter(name = "OnLine", args = [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"), model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]")]),
  model.addFilter(name = "OnLine", args = [model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"), model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]")])])
model.end()

# 2 vertices on intersection of edges
Reference = {
    model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"): True,
    model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Top]"): False,
    model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"): False,
    model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"): False,
    model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Top]"): False,
    model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Left][Box_1_1/Bottom]"): False,
    model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"): True,
    model.selection("VERTEX", "[Box_1_1/Back][Box_1_1/Right][Box_1_1/Bottom]"): False
}

model.checkFilter(Part_1_doc, model, Filters, Reference)

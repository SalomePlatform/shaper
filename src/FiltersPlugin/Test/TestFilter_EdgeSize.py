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
from SketchAPI import *
from GeomAPI import *

model.begin()
partSet = model.moduleDocument()
### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Box
Box_1 = model.addBox(Part_1_doc, 100, 50, 100)

Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "inf" , 60.0 ])])

model.end()
Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): False,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): False}

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "sup" , 60.0 ])])
model.end()

Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): True}

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "inf" , 50.0 ])])
model.end()

Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): False,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): False}

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "infEq" , 50.0 ])])
model.end()

Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): False,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): False}

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "supEq" , 50.0 ])])
model.end()

Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): True}

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "isBetween" , 50.0 , 200.0])])
model.end()

Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): True}

model.checkFilter(Part_1_doc, model, Filters, Reference)

model.begin()
Filters = model.filters(Part_1_doc, [model.addFilter(name = "EdgeSize", args = [ "isStrictlyBetween" , 50.0 , 200.0])])
model.end()

Reference = {
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Bottom]"): False,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Top]"): False,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Back][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Left]"): True,
    model.selection("EDGE", "[Box_1_1/Front][Box_1_1/Right]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Bottom]"): True,
    model.selection("EDGE", "[Box_1_1/Left][Box_1_1/Top]"): True,
    model.selection("EDGE", "[Box_1_1/Right][Box_1_1/Bottom]"): True}

model.checkFilter(Part_1_doc, model, Filters, Reference)

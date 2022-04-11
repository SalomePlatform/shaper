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

model.addParameter(Part_1_doc, "a", '10', 'longueur de l\'aile')
model.addParameter(Part_1_doc, "b", '12', 'trou à l\"origine')
model.addParameter(Part_1_doc, "c", '14', 'l\'opposé de l\"origine')

Box_1 = model.addBox(Part_1_doc, "a", "b", "c")

model.end()

assert(len(Box_1.feature().error()) == 0)
assert(model.checkPythonDump())

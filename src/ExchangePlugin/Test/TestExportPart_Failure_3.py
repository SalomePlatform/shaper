# Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
model.do()
Axis_4 = model.addAxis(partSet, model.selection("VERTEX", "Origin"), model.selection("VERTEX", "Part_1/[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]"))
Translation_1 = model.addTranslation(partSet, [model.selection("COMPOUND", "Part_1/")], model.selection("EDGE", "Axis_4"), 10)
model.end()

import os

filename = 'check_export.shaperpart'
model.removeFile(filename)

model.begin()
model.exportPart(partSet, filename)
model.end()
assert not os.path.exists(filename), "ERROR: Exporting of PartSet should fail"

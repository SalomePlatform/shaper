# Copyright (C) 2025  CEA, EDF
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

import os
from salome.kernel import salome
from salome.shaper import model
from GeomAPI import GeomAPI_Shape

salome.salome_init()

# Use the 'data' subfolder to search for the BREP files
data_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "data")

# Create part
model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

# Import first shape (part of torus)
Import_1 = model.addImport(Part_1_doc, os.path.join(data_dir, "surfaces.brep"))
model.do()
Import_1.setName("surfaces")
Import_1.result().setName("surfaces_1")
Import_1.result().subResult(0).setName("surfaces_1_1")
Import_1.result().subResult(1).setName("surfaces_1_2")
Import_1.result().subResult(2).setName("surfaces_1_3")

### Create Shell
Shell_1_objects = [model.selection("SHELL", "surfaces_1_1"),
                   model.selection("FACE", "surfaces_1_2"),
                   model.selection("SHELL", "surfaces_1_3")]
Shell_1 = model.addShell(Part_1_doc, Shell_1_objects)
Shell_1.results()[1].setName("tool")
model.do()

# Shell_1_1
LimitTolerance_1 = model.addLimitTolerance(Part_1_doc, model.selection("SHELL", "Shell_1_1"), 1e-07, True)
Split_1 = model.addSplit(Part_1_doc, [model.selection("SHELL", "LimitTolerance_1_1")], [model.selection("SHELL", "tool")], keepSubResults = True)
model.end()

model.checkResult(Split_1, model, 1, [0], [0], [2], [10], [20])

# Shell_1_3
model.undo()
model.begin()
LimitTolerance_1 = model.addLimitTolerance(Part_1_doc, model.selection("SHELL", "Shell_1_3"), 1e-07, True)
Split_1 = model.addSplit(Part_1_doc, [model.selection("SHELL", "LimitTolerance_1_1")], [model.selection("SHELL", "tool")], keepSubResults = True)
model.end()

model.checkResult(Split_1, model, 1, [0], [0], [2], [9], [18])

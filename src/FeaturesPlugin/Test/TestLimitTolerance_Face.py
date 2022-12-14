# Copyright (C) 2021-2022  CEA/DEN, EDF R&D
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
import salome
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
Import_1 = model.addImport(Part_1_doc, os.path.join(data_dir, "face_peau_bug1868.brep"))
model.do()
Import_1.setName("Torus_1")
Import_1.result().setName("Torus_1_1")

# Import second shape (planar face)
Import_2 = model.addImport(Part_1_doc, os.path.join(data_dir, "PLANFISS_bug1868.brep"))
model.do()
Import_2.setName("Face_1")
Import_2.result().setName("Face_1_1")

# Limit the tolerance of the planar face (in order to get a good intersection result)
LimitTolerance_1 = model.addLimitTolerance(Part_1_doc, model.selection("FACE", "Face_1_1"), 0.0001)

# Create intersection
Intersection_1 = model.addIntersection(Part_1_doc, [model.selection("FACE", "LimitTolerance_1_1"), model.selection("FACE", "Torus_1_1")], keepSubResults = True)
model.end()

# Result should be single edge with 2 vertices
model.checkResult(Intersection_1, model, 1, [0], [0], [0], [1], [2])

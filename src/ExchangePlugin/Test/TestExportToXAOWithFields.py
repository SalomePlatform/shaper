# Copyright (C) 2018-2019  CEA/DEN, EDF R&D
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

import tempfile
import os

def getTmpFileName(ext):
  tempdir = tempfile.gettempdir()
  tmp_file = tempfile.NamedTemporaryFile(suffix=".%s"%ext, prefix='shaper_', dir=tempdir, delete=False)
  tmp_filename = tmp_file.name
  return tmp_filename

tmp_file1 = getTmpFileName("xao")

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()
Box_1 = model.addBox(Part_1_doc, 10, 10, 10)
Field_1 = model.addField(Part_1_doc, 1, "BOOLEAN", 1, ["Comp 1"], [model.selection("FACE", "Box_1_1/Front")])
Field_1.addStep(0, 0, [[False], [False]])
Field_2_objects = [model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Top]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Left][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Bottom]"), model.selection("VERTEX", "[Box_1_1/Front][Box_1_1/Right][Box_1_1/Top]")]
Field_2 = model.addField(Part_1_doc, 1, "DOUBLE", 1, ["Comp 1"], Field_2_objects)
Field_2.addStep(0, 0, [[0], [1], [0], [0], [0]])
Field_3 = model.addField(Part_1_doc, 1, "STRING", 1, ["Comp 1"], [model.selection("SOLID", "Box_1_1")])
Field_3.addStep(0, 0, [[""], ["box"]])

Export_1 = model.exportToXAO(Part_1_doc, tmp_file1)
assert(Export_1.feature().error() == "")

model.end()

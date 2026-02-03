# Copyright (C) 2014-2026  CEA, EDF
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

# Case to check bos #45226 Import step in centimetres failed
# Problem  : Input STEP file contains vectors with length 1e-06 centimetres.
#            Converted to metres, it becomes less than Precision::Confusion()
#            and generates exception.
# Solution : Vector length checking is suppressed.

from salome.shaper import model
from inspect import getfile
from os.path import abspath, dirname, join

data_dir = abspath(join(dirname(getfile(lambda: None)), 'data'))
stepfile = join(data_dir, "face_100x100_cm.step")

model.begin()
partSet = model.moduleDocument()
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Import considering units (100 cm -> 1 m)
Import_1 = model.addImportSTEP(Part_1_doc, stepfile, True, False, False)
model.do()
Import_1.setName("face_1x1_m")
Import_1.result().setName("face_1x1_m_1")

### Import ignoring units (100 cm -> 100 m)
Import_2 = model.addImportSTEP(Part_1_doc, stepfile, False, False, False)
model.do()
Import_2.setName("face_100x100_m")
Import_2.result().setName("face_100x100_m_1")

model.end()

model.testResultsAreas(Import_1, [1])
model.testResultsAreas(Import_2, [10000])

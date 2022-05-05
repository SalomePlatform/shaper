# Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

import unittest
from salome.shaper import model

# Delta value for almost equal comparisons
DELTA = 1e-10

class SketcherTestCase(unittest.TestCase):
    def setUp(self):
        model.begin()
        partset = model.moduleDocument()
        part = model.addPart(partset).document()
        plane = model.defaultPlane("XOY")
        self.sketch = model.addSketch(part, plane)

    def tearDown(self):
        model.end()
        assert(model.checkPythonDump())
        model.reset()

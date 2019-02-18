# Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
from TestSketcher import SketcherTestCase

class SketcherSetFillet(SketcherTestCase):
    def test_fillet(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPoint(), l2.startPoint())
        self.sketch.setFillet(l1.endPoint())
        model.do()

    def test_fillet_with_radius(self):
        l1 = self.sketch.addLine(10, 10, 30, 10)
        l2 = self.sketch.addLine(10, 10, 30, 30)
        self.sketch.setCoincident(l1.startPoint(), l2.startPoint())
        self.sketch.setFilletWithRadius(l1.startPoint(), 10.0)
        model.do()

if __name__ == "__main__":
    test_program = unittest.main(verbosity=2, exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
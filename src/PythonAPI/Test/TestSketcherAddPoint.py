# Copyright (C) 2014-2024  CEA, EDF
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

class SketcherAddPoint(SketcherTestCase):
    def test_add_point(self):
        point = self.sketch.addPoint(0, 1)
        model.do()
        self.assertEqual(point.coordinates().x(), 0.0)
        self.assertEqual(point.coordinates().y(), 1.0)

    def test_modify_point(self):
        point = self.sketch.addPoint(0, 1)
        point.setCoordinates(1, 2)
        model.do()
        self.assertEqual(point.coordinates().x(), 1.0)
        self.assertEqual(point.coordinates().y(), 2.0)

if __name__ == "__main__":
    test_program = unittest.main(verbosity=2, exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
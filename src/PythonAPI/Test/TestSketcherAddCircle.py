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
from TestSketcher import SketcherTestCase

class SketcherAddCircle(SketcherTestCase):
    def test_add_cricle(self):
        circle = self.sketch.addCircle(0, 10, 20)
        model.do()
        self.assertEqual(circle.center().x(), 0.0)
        self.assertEqual(circle.center().y(), 10.0)
        self.assertEqual(circle.radius().value(), 20.0)

    def test_modify_circle(self):
        circle = self.sketch.addCircle(0, 10, 20)
        model.do()
        circle.setCenter(10, 10)
        circle.setRadius(30)
        model.do()
        self.assertEqual(circle.center().x(), 10.0)
        self.assertEqual(circle.center().y(), 10.0)
        self.assertEqual(circle.radius().value(), 30.0)


if __name__ == "__main__":
    test_program = unittest.main(verbosity=2, exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"

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
import math
import TestSketcher
from TestSketcher import SketcherTestCase

class SketcherSetEqual(SketcherTestCase):
    def test_length_equality(self):
        # Set the constraint
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setEqual(l1.result(), l2.result())
        # Commit the transaction
        model.do()
        # Check the result
        length_1 = math.sqrt(
            math.pow((l1.endPoint().x() - l1.startPoint().x()), 2) +
            math.pow((l1.endPoint().y() - l1.startPoint().y()), 2)
            )
        length_2 = math.sqrt(
            math.pow((l2.endPoint().x() - l2.startPoint().x()), 2) +
            math.pow((l2.endPoint().y() - l2.startPoint().y()), 2)
            )
        self.assertAlmostEqual(length_1, length_2, delta=TestSketcher.DELTA)

    def test_radius_equality(self):
        # Set the constraint
        circle_1 = self.sketch.addCircle(0, 0, 10.0)
        circle_2 = self.sketch.addCircle(1, 2, 25.0)
        self.sketch.setEqual(circle_1.defaultResult(), circle_2.defaultResult())
        # Commit the transaction
        model.do()
        # Check the result
        self.assertAlmostEqual(
            circle_1.radius().value(),
            circle_2.radius().value(),
            delta=TestSketcher.DELTA
            )

if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(SketcherSetEqual)
    unittest.TextTestRunner(verbosity=2).run(suite)

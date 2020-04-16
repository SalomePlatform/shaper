# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

class SketcherSetAngle(SketcherTestCase):
    def runTest(self):
        # Set the constraint
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setAngle(l1.result(), l2.result(), 30.0)
        # Commit the transaction
        model.do()
        # Check the result
        dot_product = (l1.endPoint().x() - l1.startPoint().x()) * \
                      (l2.endPoint().x() - l2.startPoint().x()) + \
                      (l1.endPoint().y() - l1.startPoint().y()) * \
                      (l2.endPoint().y() - l2.startPoint().y())
        norm_1 = math.sqrt(
            math.pow((l1.endPoint().x() - l1.startPoint().x()), 2) +
            math.pow((l1.endPoint().y() - l1.startPoint().y()), 2)
            )
        norm_2 = math.sqrt(
            math.pow((l2.endPoint().x() - l2.startPoint().x()), 2) +
            math.pow((l2.endPoint().y() - l2.startPoint().y()), 2)
            )
        angle = math.acos(dot_product / (norm_1 * norm_2))
        self.assertAlmostEqual(
            angle * 180 / math.pi, 30.0, delta=TestSketcher.DELTA
            )

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
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
import math
import TestSketcher
from TestSketcher import SketcherTestCase

class SketcherSetLength(SketcherTestCase):
    def runTest(self):
        # Set the constraint
        line = self.sketch.addLine(0, 0, 0, 1)
        self.sketch.setLength(line, 25.0)
        # Commit the transaction
        model.do()
        # Check the result
        length = math.sqrt(
            math.pow((line.endPoint().x() - line.startPoint().x()), 2) +
            math.pow((line.endPoint().y() - line.startPoint().y()), 2)
            )
        self.assertAlmostEqual(length, 25.0, delta=TestSketcher.DELTA)

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"
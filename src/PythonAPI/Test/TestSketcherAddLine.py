## Copyright (C) 2014-2017  CEA/DEN, EDF R&D
##
## This library is free software; you can redistribute it and/or
## modify it under the terms of the GNU Lesser General Public
## License as published by the Free Software Foundation; either
## version 2.1 of the License, or (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public
## License along with this library; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
##
## See http:##www.salome-platform.org/ or
## email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
##

import unittest
from salome.shaper import model
from TestSketcher import SketcherTestCase

class SketcherAddLine(SketcherTestCase):
    def test_add_line(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        model.do()
        self.assertEqual(line.startPoint().x(), line.endPoint().x())
        self.assertNotEqual(line.startPoint().y(), line.endPoint().y())

    def test_modify_line(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        model.do()
        line.setStartPoint(0, 1)
        line.setEndPoint(1, 1)
        self.assertEqual(line.startPoint().x(), 0)
        self.assertEqual(line.startPoint().y(), 1)
        self.assertEqual(line.endPoint().x(), 1)
        self.assertEqual(line.endPoint().y(), 1)


if __name__ == "__main__":
    unittest.main(verbosity=2)
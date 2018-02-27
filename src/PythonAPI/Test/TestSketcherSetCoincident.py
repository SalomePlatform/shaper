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

class SketcherSetCoincident(SketcherTestCase):
    def test_set_coincident(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPoint(), l2.startPoint())
        model.do()

    def test_none_type_arguments(self):
        l2 = self.sketch.addLine(0, 1, 1, 1)
        with self.assertRaises(TypeError):
            self.sketch.setCoincident(None, l2.startPoint())

    def test_empty_arguments(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        with self.assertRaises(TypeError):
            self.sketch.setCoincident(l1.endPoint())

if __name__ == "__main__":
    unittest.main(verbosity=2, exit=False)
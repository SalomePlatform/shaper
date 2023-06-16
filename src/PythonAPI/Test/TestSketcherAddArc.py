# Copyright (C) 2014-2023  CEA/DEN, EDF R&D
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
from salome.shaper import geom

from TestSketcher import SketcherTestCase

class SketcherAddArc(SketcherTestCase):
    def test_arc_by_coords(self):
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1, False)
        model.do()
        self.assertEqual(arc.startPoint().x(), 0)
        self.assertEqual(arc.startPoint().y(), 0)

    def test_arc_by_points(self):
        center = geom.Pnt2d(0, 1)
        start = geom.Pnt2d(0, 0)
        end = geom.Pnt2d(1, 1)
        arc = self.sketch.addArc(center, start, end, False)
        model.do()
        self.assertEqual(arc.startPoint().x(), 0)
        self.assertEqual(arc.startPoint().y(), 0)

    def test_modify_arc(self):
        # Note: arc will modify startPoint and endPoint to be in circle
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1, False)
        arc.setByCenterStartEnd(0, 0, 1, 1, -1, -1, False)
        model.do()
        self.assertEqual(arc.center().x(), 0)
        self.assertEqual(arc.center().y(), 0)
        self.assertEqual(arc.startPoint().x(), 1)
        self.assertEqual(arc.startPoint().y(), 1)
        self.assertEqual(arc.endPoint().x(), -1)
        self.assertEqual(arc.endPoint().y(), -1)


if __name__ == "__main__":
    test_program = unittest.main(verbosity=2, exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"

# Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

import ModelAPI

from salome.shaper import model

class PrimitivesAddBox(unittest.TestCase):

    def setUp(self):
        model.begin()
        # Create part
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()
        model.do()

    def tearDown(self):
        model.end()
        model.reset()

#-----------------------------------------------------------------------------
# TestCases

class PrimitivesAddBoxTestCase(PrimitivesAddBox):

    def test_add_box_by_dimensions(self):
        box = model.addBox(self.part, 50, 20, 10)
        model.do()
        self.assertEqual(box.creationMethod().value(),"BoxByDimensions")
        self.assertEqual(box.dx().value(),50)
        self.assertEqual(box.dy().value(),20)
        self.assertEqual(box.dz().value(),10)

    def test_add_box_by_two_points(self):
        point1 = model.addPoint(self.part,0,0,0).result()
        point2 = model.addPoint(self.part,10,10,10).result()
        box = model.addBox(self.part, point1, point2)
        model.do()
        self.assertEqual(box.creationMethod().value(),"BoxByTwoPoints")
        self.assertEqual(box.firstPoint().context().shape().isVertex(),True)
        self.assertEqual(box.secondPoint().context().shape().isVertex(),True)

if __name__ == "__main__":
    test_program = unittest.main(exit=False)
    assert test_program.result.wasSuccessful(), "Test failed"

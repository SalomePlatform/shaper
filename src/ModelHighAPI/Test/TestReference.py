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

import ModelAPI
import ModelHighAPI
from salome.shaper import model

class FeaturesFixture(unittest.TestCase):

    def setUp(self):
        model.begin()
        # Create part
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()
        model.do()
        self.feature = model.addPoint(self.part, 0, 0, 0)

    def tearDown(self):
        model.end()
        model.reset()


class RefAttrTestCase(FeaturesFixture):

    def test_create_default(self):
        ModelHighAPI.ModelHighAPI_Reference()

    def test_create_from_object(self):
        ModelHighAPI.ModelHighAPI_Reference(self.feature.feature())

    def test_create_from_None(self):
        ModelHighAPI.ModelHighAPI_Reference(None)


if __name__ == "__main__":
    unittest.main()

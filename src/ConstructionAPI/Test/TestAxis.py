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
import ConstructionAPI
from salome.shaper import model

class AxisTestCase(unittest.TestCase):

    def setUp(self):
        self.session = ModelAPI.ModelAPI_Session.get()
        self.doc = self.session.moduleDocument()
        self.session.startOperation()
        self.feature = self.doc.addFeature("Axis")
        self.feature.execute()
        self.session.finishOperation()

    def tearDown(self):
        assert(model.checkPythonDump())
        self.session.closeAll()

    def test_ConstructorWithDimensions(self):
        axis = ConstructionAPI.ConstructionAPI_Axis(self.feature, 10, 50, 100)
        self.assertEqual(10,axis.xDimension().value())
        self.assertEqual(50,axis.yDimension().value())
        self.assertEqual(100,axis.zDimension().value())

if __name__ == "__main__":
    unittest.main(exit=False)

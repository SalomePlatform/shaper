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

class PointTestCase(unittest.TestCase):

    def setUp(self):
        self.session = ModelAPI.ModelAPI_Session.get()
        self.doc = self.session.moduleDocument()
        self.session.startOperation()
        self.feature = self.doc.addFeature("Point")

    def tearDown(self):
        self.session.finishOperation()
        assert(model.checkPythonDump())
        self.session.closeAll()

    def test_ConstructorWithValues(self):
        point = ConstructionAPI.ConstructionAPI_Point(self.feature, 10, "20", "x + 30")
        #self.assertEqual(10, point.x().value())
        #self.assertEqual("20", point.y().text())
        #self.assertEqual("x + 30", point.z().text())

    def test_setValue(self):
        point = ConstructionAPI.ConstructionAPI_Point(self.feature)
        assert(point.x().isInitialized() == False)
        assert(point.y().isInitialized() == False)
        assert(point.z().isInitialized() == False)

        point.setByXYZ(10, "20", "x + 30")
        self.assertEqual(10, point.x().value())
        self.assertEqual("20", point.y().text())
        self.assertEqual("x + 30", point.z().text())

if __name__ == "__main__":
    unittest.main()

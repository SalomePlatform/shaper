import unittest

import ModelAPI
import ConstructionAPI
import model

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

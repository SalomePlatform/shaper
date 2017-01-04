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
    unittest.main()

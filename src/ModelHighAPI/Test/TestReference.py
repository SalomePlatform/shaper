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

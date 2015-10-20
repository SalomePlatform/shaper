import unittest
import model

class ModelTestCase(unittest.TestCase):
    def setUp(self):
        model.begin()
        partset = model.moduleDocument()
        self.part = model.addPart(partset).document()

    def tearDown(self):
        model.end()

    def test_add_sketch(self):
        plane = model.defaultPlane("XOY")
        model.addSketch(self.part, plane)

if __name__ == "__main__":
    unittest.main()

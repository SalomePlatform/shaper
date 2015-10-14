import unittest
import modeler

class ModelerTestCase(unittest.TestCase):
    def setUp(self):
        modeler.begin()
        partset = modeler.moduleDocument()
        self.part = modeler.addPart(partset).document()

    def tearDown(self):
        modeler.end()

    def test_add_sketch(self):
        plane = modeler.defaultPlane("XOY")
        modeler.addSketch(self.part, plane)

if __name__ == "__main__":
    unittest.main()

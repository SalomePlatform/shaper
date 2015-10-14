import unittest
import modeler 

class TestSketcher(unittest.TestCase):
    def test_init_sketch(self):
        modeler.begin()
        partset = modeler.moduleDocument()
        part = modeler.addPart(partset).document()
        plane = modeler.defaultPlane("XOY")
        modeler.addSketch(part, plane)
        modeler.end()

if __name__ == "__main__":
    unittest.main()

import unittest
import modeler

class SketcherTestCase(unittest.TestCase):
    def setUp(self):
        modeler.begin()
        partset = modeler.moduleDocument()
        part = modeler.addPart(partset).document()
        plane = modeler.defaultPlane("XOY")
        self.sketch = modeler.addSketch(part, plane)

    def tearDown(self):
        modeler.end()

    def test_add_line(self):
        print "add_line"
        self.sketch.addLine(0, 0, 0, 1)
    
    def test_set_coincident(self):
        print "set_coincident"
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPointData(), l2.startPointData())

if __name__ == "__main__":
    unittest.main()

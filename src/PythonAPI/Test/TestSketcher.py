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

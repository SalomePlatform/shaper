import unittest
import model

class SketcherTestCase(unittest.TestCase):
    def setUp(self):
        model.begin()
        partset = model.moduleDocument()
        part = model.addPart(partset).document()
        plane = model.defaultPlane("XOY")
        self.sketch = model.addSketch(part, plane)

    def tearDown(self):
        model.end()

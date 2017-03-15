import unittest
from salome.shaper import model
from TestSketcher import SketcherTestCase

class SketcherSetFillet(SketcherTestCase):
    # TODO: Remove tearDown method to check Python dump in super-class
    def tearDown(self):
        model.end()
        #assert(model.checkPythonDump())
        model.reset()

    def test_fillet(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPoint(), l2.startPoint())
        self.sketch.setFillet(l1.endPoint())
        model.do()

    def test_fillet_with_radius(self):
        l1 = self.sketch.addLine(10, 10, 30, 10)
        l2 = self.sketch.addLine(10, 10, 30, 30)
        self.sketch.setCoincident(l1.startPoint(), l2.startPoint())
        self.sketch.setFilletWithRadius(l1.startPoint(), 10.0)
        model.do()

if __name__ == "__main__":
    unittest.main(verbosity=2)
import unittest
from salome.shaper import model
from TestSketcher import SketcherTestCase

class SketcherSetFixed(SketcherTestCase):
    def runTest(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setFixed(circle)
        model.do()
        circle.setCenter(0, 0)
        model.do()
        self.assertEqual((circle.center().x(), circle.center().y()), (0, 10))

if __name__ == "__main__":
    unittest.main(verbosity=2)

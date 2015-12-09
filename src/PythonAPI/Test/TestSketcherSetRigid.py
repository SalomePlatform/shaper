import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetRigid(SketcherTestCase):
    def runTest(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setRigid(circle.result())
        model.do()
        circle.setCenter(0, 0)
        model.do()
        self.assertEqual((circle.center().x(), circle.center().y()), (0, 10))

if __name__ == "__main__":
    unittest.main(verbosity=2)

import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetParallel(SketcherTestCase):   
    def runTest(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setParallel(l1.result(), l2.result())

if __name__ == "__main__":
    unittest.main()
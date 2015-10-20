import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetCoincident(SketcherTestCase):   
    def runTest(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPointData(), l2.startPointData())
        model.do()

if __name__ == "__main__":
    unittest.main()
import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetFillet(SketcherTestCase):   
    def runTest(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPoint(), l2.startPoint())
        self.sketch.setFillet(l1.endPoint(), 10.0)
        model.do()

if __name__ == "__main__":
    unittest.main()
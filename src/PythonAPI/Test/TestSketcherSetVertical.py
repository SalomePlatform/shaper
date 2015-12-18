import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetVertical(SketcherTestCase):   
    def runTest(self):
        line = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setVertical(line.result())
        model.do()
        self.assertEqual(line.startPoint().x(), line.endPoint().x())

if __name__ == "__main__":
    unittest.main()
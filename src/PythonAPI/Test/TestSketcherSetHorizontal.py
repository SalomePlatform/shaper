import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetHorizontal(SketcherTestCase):   
    def runTest(self):
        line = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setHorizontal(line.result())
        model.do()
        self.assertEqual(line.startPointData().y(), line.endPointData().y())

if __name__ == "__main__":
    unittest.main()
import unittest
import model
import math
import TestSketcher
from TestSketcher import SketcherTestCase

class SketcherSetLength(SketcherTestCase):   
    def runTest(self):
        # Set the constraint
        line = self.sketch.addLine(0, 0, 0, 1)
        self.sketch.setLength(line.result(), 25.0)
        # Commit the transaction
        model.do()
        # Check the result
        length = math.sqrt(
            math.pow((line.endPoint().x() - line.startPoint().x()), 2) + 
            math.pow((line.endPoint().y() - line.startPoint().y()), 2)
            )
        self.assertAlmostEqual(length, 25.0, delta=TestSketcher.DELTA)
        
if __name__ == "__main__":
    unittest.main()
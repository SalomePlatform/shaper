import unittest
import model
import TestSketcher
from TestSketcher import SketcherTestCase

class SketcherSetPerpendicular(SketcherTestCase):
    """Test case for prependicular constraint testing."""
    def runTest(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setPerpendicular(l1.result(), l2.result())
        model.do()
        
        dot_product = (l1.endPoint().x() - l1.startPoint().x()) * \
                      (l2.endPoint().x() - l2.startPoint().x()) + \
                      (l1.endPoint().y() - l1.startPoint().y()) * \
                      (l2.endPoint().y() - l2.startPoint().y())
        self.assertAlmostEqual(dot_product, 0.0, delta=TestSketcher.DELTA)

if __name__ == "__main__":
    unittest.main()
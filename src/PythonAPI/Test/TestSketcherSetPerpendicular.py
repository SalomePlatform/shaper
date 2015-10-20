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
        
        dot_product = (l1.endPointData().x() - l1.startPointData().x()) * \
                      (l2.endPointData().x() - l2.startPointData().x()) + \
                      (l1.endPointData().y() - l1.startPointData().y()) * \
                      (l2.endPointData().y() - l2.startPointData().y())
        self.assertAlmostEqual(dot_product, 0.0, delta=TestSketcher.DELTA)

if __name__ == "__main__":
    unittest.main()
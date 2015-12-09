import unittest
import model
import math
import TestSketcher
from TestSketcher import SketcherTestCase

class SketcherSetAngle(SketcherTestCase):   
    def runTest(self):
        # Set the constraint
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setAngle(l1.result(), l2.result(), 30.0)
        # Commit the transaction
        model.do()
        # Check the result
        dot_product = (l1.endPointData().x() - l1.startPointData().x()) * \
                      (l2.endPointData().x() - l2.startPointData().x()) + \
                      (l1.endPointData().y() - l1.startPointData().y()) * \
                      (l2.endPointData().y() - l2.startPointData().y())
        norm_1 = math.sqrt(
            math.pow((l1.endPointData().x() - l1.startPointData().x()), 2) + 
            math.pow((l1.endPointData().y() - l1.startPointData().y()), 2)
            )
        norm_2 = math.sqrt(
            math.pow((l2.endPointData().x() - l2.startPointData().x()), 2) + 
            math.pow((l2.endPointData().y() - l2.startPointData().y()), 2)
            )
        angle = math.acos(dot_product / (norm_1 * norm_2))
        self.assertAlmostEqual(
            angle * 180 / math.pi, 30.0, delta=TestSketcher.DELTA
            )
        
if __name__ == "__main__":
    unittest.main()
import unittest
import model
import math
import TestSketcher
from TestSketcher import SketcherTestCase

class SketcherSetEqual(SketcherTestCase):
    def test_length_equality(self):
        # Set the constraint
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 0, 1, 1)
        self.sketch.setEqual(l1.result(), l2.result())
        # Commit the transaction
        model.do()
        # Check the result
        length_1 = math.sqrt(
            math.pow((l1.endPointData().x() - l1.startPointData().x()), 2) +
            math.pow((l1.endPointData().y() - l1.startPointData().y()), 2)
            )
        length_2 = math.sqrt(
            math.pow((l2.endPointData().x() - l2.startPointData().x()), 2) +
            math.pow((l2.endPointData().y() - l2.startPointData().y()), 2)
            )
        self.assertAlmostEqual(length_1, length_2, delta=TestSketcher.DELTA)

    def test_radius_equality(self):
        # Set the constraint
        circle_1 = self.sketch.addCircle(0, 0, 10.0)
        circle_2 = self.sketch.addCircle(1, 2, 25.0)
        self.sketch.setEqual(circle_1.result(), circle_2.result())
        # Commit the transaction
        model.do()
        # Check the result
        self.assertAlmostEqual(
            circle_1.radiusData().value(),
            circle_2.radiusData().value(),
            delta=TestSketcher.DELTA
            )

if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(SketcherSetEqual)
    unittest.TextTestRunner(verbosity=2).run(suite)

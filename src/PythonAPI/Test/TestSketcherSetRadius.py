import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetRadius(SketcherTestCase):
    def test_set_radius(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setRadius(circle, 30)
        model.do()
        self.assertEqual(circle.radius().value(), 30.0)

    def test_modify_radius(self):
        circle = self.sketch.addCircle(0, 10, 20)
        radius_constraint = self.sketch.setRadius(circle, 30)
        model.do()
        self.sketch.setValue(radius_constraint, 25)
        model.do()
        self.assertEqual(circle.radius().value(), 25.0)

if __name__ == "__main__":
    unittest.main(verbosity=2)
import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetRadius(SketcherTestCase):   
    def test_set_radius(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setRadius(circle.result(), 30)
        model.do()
        self.assertEqual(circle.radius(), 30.0)
        
    def test_modify_radius(self):
        circle = self.sketch.addCircle(0, 10, 20)
        radius_constraint = self.sketch.setRadius(circle.result(), 30)
        model.do()
        self.sketch.setValue(radius_constraint, 20)
        model.do()
        self.assertEqual(circle.radius(), 20.0)
        
if __name__ == "__main__":
    unittest.main(verbosity=2)
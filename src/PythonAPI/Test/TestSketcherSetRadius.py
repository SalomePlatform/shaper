import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetRadius(SketcherTestCase):   
    def test_set_radius(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setRadius(circle.result(), 30)
        model.do()
        self.assertEqual(circle.radiusData().value(), 30.0)
        
    def test_modify_radius(self):
        circle = self.sketch.addCircle(0, 10, 20)
        radius = self.sketch.setRadius(circle.result(), 30)
        model.do()
        self.sketch.setValue(radius, 20)
        model.do()
        self.assertEqual(circle.radiusData().value(), 20.0)
        
if __name__ == "__main__":
    unittest.main(verbosity=2)
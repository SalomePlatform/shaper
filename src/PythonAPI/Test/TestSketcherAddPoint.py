import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherAddPoint(SketcherTestCase):    
    def test_add_point(self):
        point = self.sketch.addPoint(0, 1)
        model.do()
        self.assertEqual(point.pointData().x(), 0.0)        
        self.assertEqual(point.pointData().y(), 1.0)
        
    def test_modify_point(self):
        point = self.sketch.addPoint(0, 1)
        point.setValue(1, 2)
        model.do()
        self.assertEqual(point.pointData().x(), 1.0)        
        self.assertEqual(point.pointData().y(), 2.0)
        
    
if __name__ == "__main__":
    unittest.main(verbosity=2)
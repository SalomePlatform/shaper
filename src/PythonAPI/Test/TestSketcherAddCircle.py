import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherAddCircle(SketcherTestCase):    
    def runTest(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.assertEqual(circle.centerData().x(), 0.0)        
        self.assertEqual(circle.centerData().y(), 10.0)
        self.assertEqual(circle.radiusData().value(), 20.0)
        
    
if __name__ == "__main__":
    unittest.main()
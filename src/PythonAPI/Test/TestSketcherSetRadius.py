import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetRadius(SketcherTestCase):   
    def runTest(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setRadius(circle.result(), 30)
        model.do()
        self.assertEqual(circle.radiusData().value(), 30.0)
        
if __name__ == "__main__":
    unittest.main()
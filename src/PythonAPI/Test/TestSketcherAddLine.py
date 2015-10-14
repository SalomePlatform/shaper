import unittest
import modeler
from TestSketcher import SketcherTestCase

class SketcherAddLineTestCase(SketcherTestCase):    
    def runTest(self):
        self.sketch.addLine(0, 0, 0, 1)
    
if __name__ == "__main__":
    unittest.main()
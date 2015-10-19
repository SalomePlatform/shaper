import unittest
import modeler
from TestSketcher import SketcherTestCase

class SketcherAddLine(SketcherTestCase):    
    def runTest(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        self.assertEqual(line.startPointData().x(), line.endPointData().x())        
        self.assertNotEqual(line.startPointData().y(), line.endPointData().y())
        
    
if __name__ == "__main__":
    unittest.main()
import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetRigid(SketcherTestCase):   
    def runTest(self):
        circle = self.sketch.addCircle(0, 10, 20)
        self.sketch.setRigid(circle.result())
        model.do()
        with self.assertRaises(Exception):
            circle.setCenter(0, 0)
            model.do()
        
if __name__ == "__main__":
    unittest.main(verbosity=2)
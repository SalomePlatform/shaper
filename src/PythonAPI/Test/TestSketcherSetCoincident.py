import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherSetCoincident(SketcherTestCase):   
    def test_set_coincident(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        l2 = self.sketch.addLine(0, 1, 1, 1)
        self.sketch.setCoincident(l1.endPoint(), l2.startPoint())
        model.do()
    
    def test_none_type_arguments(self):
        l2 = self.sketch.addLine(0, 1, 1, 1)
        with self.assertRaises(TypeError):
            self.sketch.setCoincident(None, l2.startPoint())
        
    def test_empty_arguments(self):
        l1 = self.sketch.addLine(0, 0, 0, 1)
        with self.assertRaises(TypeError):
            self.sketch.setCoincident(l1.endPoint())

if __name__ == "__main__":
    unittest.main(verbosity=2)
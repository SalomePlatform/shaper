import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherAddLine(SketcherTestCase):    
    def test_add_line(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        model.do()
        self.assertEqual(line.getStartPoint().x(), line.getEndPoint().x())        
        self.assertNotEqual(line.getStartPoint().y(), line.getEndPoint().y())

    def test_modify_line(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        model.do()
        line.setStartPoint(0, 1)
        line.setEndPoint(1, 1)
        self.assertEqual(line.getStartPoint().x(), 0)
        self.assertEqual(line.getStartPoint().y(), 1)
        self.assertEqual(line.getEndPoint().x(), 1)
        self.assertEqual(line.getEndPoint().y(), 1)


if __name__ == "__main__":
    unittest.main(verbosity=2)
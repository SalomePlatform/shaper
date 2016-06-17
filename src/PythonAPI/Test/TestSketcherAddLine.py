import unittest
import model
from TestSketcher import SketcherTestCase

class SketcherAddLine(SketcherTestCase):
    def test_add_line(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        model.do()
        self.assertEqual(line.startPoint().x(), line.endPoint().x())
        self.assertNotEqual(line.startPoint().y(), line.endPoint().y())

    def test_modify_line(self):
        line = self.sketch.addLine(0, 0, 0, 1)
        model.do()
        line.setStartPoint(0, 1)
        line.setEndPoint(1, 1)
        self.assertEqual(line.startPoint().x(), 0)
        self.assertEqual(line.startPoint().y(), 1)
        self.assertEqual(line.endPoint().x(), 1)
        self.assertEqual(line.endPoint().y(), 1)


if __name__ == "__main__":
    unittest.main(verbosity=2)
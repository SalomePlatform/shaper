import unittest
from salome.shaper import model
from TestSketcher import SketcherTestCase

class SketcherAddPoint(SketcherTestCase):
    def test_add_point(self):
        point = self.sketch.addPoint(0, 1)
        model.do()
        self.assertEqual(point.coordinates().x(), 0.0)
        self.assertEqual(point.coordinates().y(), 1.0)

    def test_modify_point(self):
        point = self.sketch.addPoint(0, 1)
        point.setCoordinates(1, 2)
        model.do()
        self.assertEqual(point.coordinates().x(), 1.0)
        self.assertEqual(point.coordinates().y(), 2.0)

if __name__ == "__main__":
    unittest.main(verbosity=2)
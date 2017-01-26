import unittest

from salome.shaper import model
from salome.shaper import geom

from TestSketcher import SketcherTestCase

class SketcherAddArc(SketcherTestCase):
    def test_arc_by_coords(self):
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1, False)
        model.do()
        self.assertEqual(arc.startPoint().x(), 0)
        self.assertEqual(arc.startPoint().y(), 0)

    def test_arc_by_points(self):
        center = geom.Pnt2d(0, 1)
        start = geom.Pnt2d(0, 0)
        end = geom.Pnt2d(1, 1)
        arc = self.sketch.addArc(center, start, end, False)
        model.do()
        self.assertEqual(arc.startPoint().x(), 0)
        self.assertEqual(arc.startPoint().y(), 0)

    def test_modify_arc(self):
        # Note: arc will modify startPoint and endPoint to be in circle
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1, False)
        arc.setByCenterStartEnd(0, 0, 1, 1, -1, -1, False)
        model.do()
        self.assertEqual(arc.center().x(), 0)
        self.assertEqual(arc.center().y(), 0)
        self.assertEqual(arc.startPoint().x(), 1)
        self.assertEqual(arc.startPoint().y(), 1)
        self.assertEqual(arc.endPoint().x(), -1)
        self.assertEqual(arc.endPoint().y(), -1)


if __name__ == "__main__":
    unittest.main(verbosity=2)

import unittest

import modeler
import geom
from modeler import WrongNumberOfArguments

from TestSketcher import SketcherTestCase

class SketcherAddArc(SketcherTestCase):    
    def test_arc_by_coords(self):
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1)
        self.assertEqual(arc.startPointData().x(), 0)        
        self.assertEqual(arc.startPointData().y(), 0)
    
    def test_arc_by_points(self):
        center = geom.Pnt2d(0, 1)
        start = geom.Pnt2d(0, 0)
        end = geom.Pnt2d(0, 1)
        arc = self.sketch.addArc(center, start, end)
        self.assertEqual(arc.startPointData().x(), 0)        
        self.assertEqual(arc.startPointData().y(), 0)
    
    def test_number_of_args(self):
        with self.assertRaises(WrongNumberOfArguments):
            self.sketch.addArc(0, 1, 1, 1)
        with self.assertRaises(WrongNumberOfArguments):
            self.sketch.addArc(0, 1)
        
    
if __name__ == "__main__":
    suite = unittest.TestLoader().loadTestsFromTestCase(SketcherAddArc)
    unittest.TextTestRunner(verbosity=2).run(suite)
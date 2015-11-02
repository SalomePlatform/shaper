import unittest

import model
import geom
from model import WrongNumberOfArguments

from TestSketcher import SketcherTestCase

class SketcherAddArc(SketcherTestCase):    
    def test_arc_by_coords(self):
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1)
        model.do()
        self.assertEqual(arc.startPointData().x(), 0)        
        self.assertEqual(arc.startPointData().y(), 0)
    
    def test_arc_by_points(self):
        center = geom.Pnt2d(0, 1)
        start = geom.Pnt2d(0, 0)
        end = geom.Pnt2d(1, 1)
        arc = self.sketch.addArc(center, start, end)
        model.do()
        self.assertEqual(arc.startPointData().x(), 0)        
        self.assertEqual(arc.startPointData().y(), 0)
    
    def test_number_of_args(self):
        with self.assertRaises(WrongNumberOfArguments):
            self.sketch.addArc(0, 1, 1, 1)
        with self.assertRaises(WrongNumberOfArguments):
            self.sketch.addArc(0, 1)
            
    def test_modify_arc(self):
        arc = self.sketch.addArc(0, 1, 0, 0, 1, 1)
        arc.setCenter(0, 0)
        arc.setStartPoint(-1.0, 0)
        arc.setEndPoint(0, 1.0)
        model.do()
        self.assertEqual(arc.centerData().x(), 0)        
        self.assertEqual(arc.centerData().y(), 0)
        self.assertEqual(arc.startPointData().x(), -1)        
        self.assertEqual(arc.startPointData().y(), 0)
        self.assertEqual(arc.endPointData().x(), 0)        
        self.assertEqual(arc.endPointData().y(), 1)
        
    
if __name__ == "__main__":
    unittest.main(verbosity=2)